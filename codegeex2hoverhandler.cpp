// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0+ OR GPL-3.0 WITH Qt-GPL-exception-1.0

#include "codegeex2hoverhandler.h"

#include "codegeex2client.h"
#include "codegeex2suggestion.h"
#include "codegeex2tr.h"

#include <texteditor/textdocument.h>
#include <texteditor/textdocumentlayout.h>
#include <texteditor/texteditor.h>

#include <utils/tooltip/tooltip.h>
#include <utils/utilsicons.h>

#include <QPushButton>
#include <QScopeGuard>
#include <QToolBar>
#include <QToolButton>

using namespace TextEditor;
using namespace LanguageServerProtocol;
using namespace Utils;

namespace CodeGeeX2::Internal {

class CodeGeeX2CompletionToolTip : public QToolBar
{
public:
    CodeGeeX2CompletionToolTip(QList<Completion> completions,
                             int currentCompletion,
                             TextEditorWidget *editor)
        : m_numberLabel(new QLabel)
        , m_completions(completions)
        , m_currentCompletion(std::max(0, std::min<int>(currentCompletion, completions.size() - 1)))
        , m_editor(editor)
    {
        auto prev = addAction(Utils::Icons::PREV_TOOLBAR.icon(),
                              Tr::tr("Select Previous CodeGeeX2 Suggestion"));
        prev->setEnabled(m_completions.size() > 1);
        addWidget(m_numberLabel);
        auto next = addAction(Utils::Icons::NEXT_TOOLBAR.icon(),
                              Tr::tr("Select Next CodeGeeX2 Suggestion"));
        next->setEnabled(m_completions.size() > 1);

        auto apply = addAction(Tr::tr("Apply (%1)").arg(QKeySequence(Qt::Key_Tab).toString()));
        auto applyWord = addAction(
            Tr::tr("Apply Word (%1)").arg(QKeySequence(QKeySequence::MoveToNextWord).toString()));

        connect(prev, &QAction::triggered, this, &CodeGeeX2CompletionToolTip::selectPrevious);
        connect(next, &QAction::triggered, this, &CodeGeeX2CompletionToolTip::selectNext);
        connect(apply, &QAction::triggered, this, &CodeGeeX2CompletionToolTip::apply);
        connect(applyWord, &QAction::triggered, this, &CodeGeeX2CompletionToolTip::applyWord);

        updateLabels();
    }

private:
    void updateLabels()
    {
        m_numberLabel->setText(Tr::tr("%1 of %2")
                                   .arg(m_currentCompletion + 1)
                                   .arg(m_completions.count()));
    }

    void selectPrevious()
    {
        --m_currentCompletion;
        if (m_currentCompletion < 0)
            m_currentCompletion = m_completions.size() - 1;
        setCurrentCompletion();
    }

    void selectNext()
    {
        ++m_currentCompletion;
        if (m_currentCompletion >= m_completions.size())
            m_currentCompletion = 0;
        setCurrentCompletion();
    }

    void setCurrentCompletion()
    {
        updateLabels();
        if (TextSuggestion *suggestion = m_editor->currentSuggestion())
            suggestion->reset();
        m_editor->insertSuggestion(std::make_unique<CodeGeeX2Suggestion>(m_completions,
                                                                       m_editor->document(),
                                                                       m_currentCompletion));
    }

    void apply()
    {
        if (TextSuggestion *suggestion = m_editor->currentSuggestion()) {
            if (!suggestion->apply())
                return;
        }
        ToolTip::hide();
    }

    void applyWord()
    {
        if (TextSuggestion *suggestion = m_editor->currentSuggestion()) {
            if (!suggestion->applyWord(m_editor))
                return;
        }
        ToolTip::hide();
    }

    QLabel *m_numberLabel;
    QList<Completion> m_completions;
    int m_currentCompletion = 0;
    TextEditorWidget *m_editor;
};

void CodeGeeX2HoverHandler::identifyMatch(TextEditorWidget *editorWidget,
                                        int pos,
                                        ReportPriority report)
{
    QScopeGuard cleanup([&] { report(Priority_None); });
    if (!editorWidget->suggestionVisible())
        return;

    QTextCursor cursor(editorWidget->document());
    cursor.setPosition(pos);
    m_block = cursor.block();
    auto *suggestion = dynamic_cast<CodeGeeX2Suggestion *>(TextDocumentLayout::suggestion(m_block));

    if (!suggestion)
        return;

    const QList<Completion> completions = suggestion->completions();
    if (completions.isEmpty())
        return;

    cleanup.dismiss();
    report(Priority_Suggestion);
}

void CodeGeeX2HoverHandler::operateTooltip(TextEditorWidget *editorWidget, const QPoint &point)
{
    Q_UNUSED(point)
    auto *suggestion = dynamic_cast<CodeGeeX2Suggestion *>(TextDocumentLayout::suggestion(m_block));

    if (!suggestion)
        return;

    auto tooltipWidget = new CodeGeeX2CompletionToolTip(suggestion->completions(),
                                                      suggestion->currentCompletion(),
                                                      editorWidget);

    const QRect cursorRect = editorWidget->cursorRect(editorWidget->textCursor());
    QPoint pos = editorWidget->viewport()->mapToGlobal(cursorRect.topLeft())
                 - Utils::ToolTip::offsetFromPosition();
    pos.ry() -= tooltipWidget->sizeHint().height();
    ToolTip::show(pos, tooltipWidget, editorWidget);
}

} // namespace CodeGeeX2::Internal
