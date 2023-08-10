#include "codegeex2plugin.h"

#include "codegeex2client.h"
#include "codegeex2constants.h"
#include "codegeex2icons.h"
#include "codegeex2optionspage.h"
#include "codegeex2projectpanel.h"
#include "codegeex2settings.h"
#include "codegeex2suggestion.h"
#include "codegeex2tr.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/statusbarmanager.h>

#include <languageclient/languageclientmanager.h>

#include <projectexplorer/projectpanelfactory.h>

#include <texteditor/textdocumentlayout.h>
#include <texteditor/texteditor.h>

#include <QAction>
#include <QToolButton>

using namespace Utils;
using namespace Core;
using namespace ProjectExplorer;

namespace CodeGeeX2 {
namespace Internal {

enum Direction { Previous, Next };
void cycleSuggestion(TextEditor::TextEditorWidget *editor, Direction direction)
{
    QTextBlock block = editor->textCursor().block();
    if (auto *suggestion = dynamic_cast<CodeGeeX2Suggestion *>(
            TextEditor::TextDocumentLayout::suggestion(block))) {
        int index = suggestion->currentCompletion();
        if (direction == Previous)
            --index;
        else
            ++index;
        if (index < 0)
            index = suggestion->completions().count() - 1;
        else if (index >= suggestion->completions().count())
            index = 0;
        suggestion->reset();
        editor->insertSuggestion(std::make_unique<CodeGeeX2Suggestion>(suggestion->completions(),
                                                                     editor->document(),
                                                                     index));
    }
}

void CodeGeeX2Plugin::initialize()
{
    CodeGeeX2Settings::instance().readSettings(ICore::settings());

    restartClient();

    connect(&CodeGeeX2Settings::instance(),
            &CodeGeeX2Settings::applied,
            this,
            &CodeGeeX2Plugin::restartClient);

    QAction *requestAction = new QAction(this);
    requestAction->setText(Tr::tr("Request CodeGeeX2 Suggestion"));
    requestAction->setToolTip(
        Tr::tr("Request CodeGeeX2 suggestion at the current editor's cursor position."));

    connect(requestAction, &QAction::triggered, this, [this] {
        if (auto editor = TextEditor::TextEditorWidget::currentTextEditorWidget()) {
            if (m_client && m_client->reachable())
                m_client->requestCompletions(editor);
        }
    });

    ActionManager::registerAction(requestAction, Constants::CODEGEEX2_REQUEST_SUGGESTION);

    QAction *nextSuggestionAction = new QAction(this);
    nextSuggestionAction->setText(Tr::tr("Show next CodeGeeX2 Suggestion"));
    nextSuggestionAction->setToolTip(Tr::tr(
        "Cycles through the received CodeGeeX2 Suggestions showing the next available Suggestion."));

    connect(nextSuggestionAction, &QAction::triggered, this, [] {
        if (auto editor = TextEditor::TextEditorWidget::currentTextEditorWidget())
            cycleSuggestion(editor, Next);
    });

    ActionManager::registerAction(nextSuggestionAction, Constants::CODEGEEX2_NEXT_SUGGESTION);

    QAction *previousSuggestionAction = new QAction(this);
    previousSuggestionAction->setText(Tr::tr("Show previos CodeGeeX2 Suggestion"));
    previousSuggestionAction->setToolTip(Tr::tr("Cycles through the received CodeGeeX2 Suggestions "
                                                "showing the previous available Suggestion."));

    connect(previousSuggestionAction, &QAction::triggered, this, [] {
        if (auto editor = TextEditor::TextEditorWidget::currentTextEditorWidget())
            cycleSuggestion(editor, Previous);
    });

    ActionManager::registerAction(previousSuggestionAction, Constants::CODEGEEX2_PREVIOUS_SUGGESTION);

    QAction *disableAction = new QAction(this);
    disableAction->setText(Tr::tr("Disable CodeGeeX2"));
    disableAction->setToolTip(Tr::tr("Disable CodeGeeX2."));
    connect(disableAction, &QAction::triggered, this, [] {
        CodeGeeX2Settings::instance().enableCodeGeeX2.setValue(true);
        CodeGeeX2Settings::instance().apply();
    });
    ActionManager::registerAction(disableAction, Constants::CODEGEEX2_DISABLE);

    QAction *enableAction = new QAction(this);
    enableAction->setText(Tr::tr("Enable CodeGeeX2"));
    enableAction->setToolTip(Tr::tr("Enable CodeGeeX2."));
    connect(enableAction, &QAction::triggered, this, [] {
        CodeGeeX2Settings::instance().enableCodeGeeX2.setValue(false);
        CodeGeeX2Settings::instance().apply();
    });
    ActionManager::registerAction(enableAction, Constants::CODEGEEX2_ENABLE);

    QAction *toggleAction = new QAction(this);
    toggleAction->setText(Tr::tr("Toggle CodeGeeX2"));
    toggleAction->setCheckable(true);
    toggleAction->setChecked(CodeGeeX2Settings::instance().enableCodeGeeX2.value());
    toggleAction->setIcon(CODEGEEX2_ICON.icon());
    connect(toggleAction, &QAction::toggled, this, [](bool checked) {
        CodeGeeX2Settings::instance().enableCodeGeeX2.setValue(checked);
        CodeGeeX2Settings::instance().apply();
    });

    ActionManager::registerAction(toggleAction, Constants::CODEGEEX2_TOGGLE);

    auto updateActions = [toggleAction, requestAction] {
        const bool enabled = CodeGeeX2Settings::instance().enableCodeGeeX2.value();
        toggleAction->setToolTip(enabled ? Tr::tr("Disable CodeGeeX2.") : Tr::tr("Enable CodeGeeX2."));
        toggleAction->setChecked(enabled);
        requestAction->setEnabled(enabled);
    };

    connect(&CodeGeeX2Settings::instance().enableCodeGeeX2,
            &BoolAspect::valueChanged,
            this,
            updateActions);

    updateActions();

    auto toggleButton = new QToolButton;
    toggleButton->setDefaultAction(toggleAction);
    StatusBarManager::addStatusBarWidget(toggleButton, StatusBarManager::RightCorner);

    auto panelFactory = new ProjectPanelFactory;
    panelFactory->setPriority(1000);
    panelFactory->setDisplayName(Tr::tr("CodeGeeX2"));
    panelFactory->setCreateWidgetFunction(&Internal::createCodeGeeX2ProjectPanel);
    ProjectPanelFactory::registerFactory(panelFactory);
}

void CodeGeeX2Plugin::extensionsInitialized()
{
    (void)CodeGeeX2OptionsPage::instance();
}

void CodeGeeX2Plugin::restartClient()
{
    LanguageClient::LanguageClientManager::shutdownClient(m_client);

    m_client = new CodeGeeX2Client();
}

ExtensionSystem::IPlugin::ShutdownFlag CodeGeeX2Plugin::aboutToShutdown()
{
    if (!m_client)
        return SynchronousShutdown;
    connect(m_client, &QObject::destroyed, this, &IPlugin::asynchronousShutdownFinished);
    return AsynchronousShutdown;
}

} // namespace Internal
} // namespace CodeGeeX2
