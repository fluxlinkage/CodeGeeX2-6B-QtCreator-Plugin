#include "codegeex2client.h"
#include "codegeex2constants.h"
#include "codegeex2settings.h"
#include "codegeex2suggestion.h"
#include "codegeex2clientinterface.h"

#include <languageclient/languageclientinterface.h>
#include <languageclient/languageclientmanager.h>
#include <languageclient/languageclientsettings.h>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/editormanager/editormanager.h>

#include <projectexplorer/projectmanager.h>

#include <utils/filepath.h>

#include <texteditor/textdocumentlayout.h>
#include <texteditor/texteditor.h>

#include <languageserverprotocol/lsptypes.h>

#include <QTimer>
#include <QToolButton>

using namespace LanguageServerProtocol;
using namespace TextEditor;
using namespace Utils;
using namespace ProjectExplorer;
using namespace Core;

namespace CodeGeeX2::Internal {

CodeGeeX2Client::CodeGeeX2Client()
    : LanguageClient::Client(new CodeGeeX2ClientInterface())
{
    setName("CodeGeeX2");
    LanguageClient::LanguageFilter langFilter;

    langFilter.filePattern = {"*"};

    setSupportedLanguage(langFilter);
    start();

    auto openDoc = [this](IDocument *document) {
        if (auto *textDocument = qobject_cast<TextDocument *>(document))
            openDocument(textDocument);
    };

    connect(EditorManager::instance(), &EditorManager::documentOpened, this, openDoc);
    connect(EditorManager::instance(),
            &EditorManager::documentClosed,
            this,
            [this](IDocument *document) {
                if (auto textDocument = qobject_cast<TextDocument *>(document))
                    closeDocument(textDocument);
            });

    for (IDocument *doc : DocumentModel::openedDocuments())
        openDoc(doc);
}

CodeGeeX2Client::~CodeGeeX2Client()
{
    for (IEditor *editor : DocumentModel::editorsForOpenedDocuments()) {
        if (auto textEditor = qobject_cast<BaseTextEditor *>(editor))
            textEditor->editorWidget()->removeHoverHandler(&m_hoverHandler);
    }
}

void CodeGeeX2Client::openDocument(TextDocument *document)
{
    auto project = ProjectManager::projectForFile(document->filePath());
    if (!isEnabled(project))
        return;

    Client::openDocument(document);
    connect(document,
            &TextDocument::contentsChangedWithPosition,
            this,
            [this, document](int position, int charsRemoved, int charsAdded) {
                Q_UNUSED(charsRemoved)
                if (!CodeGeeX2Settings::instance().autoComplete())
                    return;

                auto project = ProjectManager::projectForFile(document->filePath());
                if (!isEnabled(project))
                    return;

                auto textEditor = BaseTextEditor::currentTextEditor();
                if (!textEditor || textEditor->document() != document)
                    return;
                TextEditorWidget *widget = textEditor->editorWidget();
                if (widget->multiTextCursor().hasMultipleCursors())
                    return;
                const int cursorPosition = widget->textCursor().position();
                if (cursorPosition < position || cursorPosition > position + charsAdded)
                    return;
                scheduleRequest(widget);
            });
}

void CodeGeeX2Client::scheduleRequest(TextEditorWidget *editor)
{
    cancelRunningRequest(editor);

    if (!m_scheduledRequests.contains(editor)) {
        auto timer = new QTimer(this);
        timer->setSingleShot(true);
        connect(timer, &QTimer::timeout, this, [this, editor]() {
            if (m_scheduledRequests[editor].cursorPosition == editor->textCursor().position())
                requestCompletions(editor);
        });
        connect(editor, &TextEditorWidget::destroyed, this, [this, editor]() {
            delete m_scheduledRequests.take(editor).timer;
            cancelRunningRequest(editor);
        });
        connect(editor, &TextEditorWidget::cursorPositionChanged, this, [this, editor] {
            cancelRunningRequest(editor);
        });
        m_scheduledRequests.insert(editor, {editor->textCursor().position(), timer});
    } else {
        m_scheduledRequests[editor].cursorPosition = editor->textCursor().position();
    }
    m_scheduledRequests[editor].timer->start(500);
}

void CodeGeeX2Client::requestCompletions(TextEditorWidget *editor)
{
    auto project = ProjectManager::projectForFile(editor->textDocument()->filePath());

    if (!isEnabled(project))
        return;

    Utils::MultiTextCursor cursor = editor->multiTextCursor();
    if (cursor.hasMultipleCursors() || cursor.hasSelection() || editor->suggestionVisible())
        return;

    const Utils::FilePath filePath = editor->textDocument()->filePath();
    GetCompletionRequest request{
        {TextDocumentIdentifier(hostPathToServerUri(filePath)),
         documentVersion(filePath),
         Position(cursor.mainCursor()),editor->textDocument()->plainText(),editor->position()}};
    request.setResponseCallback([this, editor = QPointer<TextEditorWidget>(editor)](
                                    const GetCompletionRequest::Response &response) {
        QTC_ASSERT(editor, return);
        handleCompletions(response, editor);
    });
    m_runningRequests[editor] = request;
    sendMessage(request);
}

void CodeGeeX2Client::handleCompletions(const GetCompletionRequest::Response &response,
                                      TextEditorWidget *editor)
{
    if (response.error())
        log(*response.error());

    int requestPosition = -1;
    if (const auto requestParams = m_runningRequests.take(editor).params())
        requestPosition = requestParams->position().toPositionInDocument(editor->document());

    const Utils::MultiTextCursor cursors = editor->multiTextCursor();
    if (cursors.hasMultipleCursors())
        return;

    if (cursors.hasSelection() || cursors.mainCursor().position() != requestPosition)
        return;

    if (const std::optional<GetCompletionResponse> result = response.result()) {
        auto isValidCompletion = [](const Completion &completion) {
            return completion.isValid() && !completion.text().trimmed().isEmpty();
        };
        QList<Completion> completions = Utils::filtered(result->completions().toListOrEmpty(),
                                                              isValidCompletion);

        // remove trailing whitespaces from the end of the completions
        for (Completion &completion : completions) {
            const LanguageServerProtocol::Range range = completion.range();
            if (range.start().line() != range.end().line())
                continue; // do not remove trailing whitespaces for multi-line replacements

            const QString completionText = completion.text();
            const int end = int(completionText.size()) - 1; // empty strings have been removed above
            int delta = 0;
            while (delta <= end && completionText[end - delta].isSpace())
                ++delta;

            if (delta > 0)
                completion.setText(completionText.chopped(delta));
        }
        if (completions.isEmpty())
            return;
        editor->insertSuggestion(
            std::make_unique<CodeGeeX2Suggestion>(completions, editor->document()));
        editor->addHoverHandler(&m_hoverHandler);
    }
}

void CodeGeeX2Client::cancelRunningRequest(TextEditor::TextEditorWidget *editor)
{
    auto it = m_runningRequests.find(editor);
    if (it == m_runningRequests.end())
        return;
    cancelRequest(it->id());
    m_runningRequests.erase(it);
}

bool CodeGeeX2Client::canOpenProject(Project *project)
{
    return isEnabled(project);
}

bool CodeGeeX2Client::isEnabled(Project *project)
{
    if (!project)
        return CodeGeeX2Settings::instance().enableCodeGeeX2();

    CodeGeeX2ProjectSettings settings(project);
    return settings.isEnabled();
}

} // namespace CodeGeeX2::Internal
