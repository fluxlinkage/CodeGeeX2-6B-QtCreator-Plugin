// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0+ OR GPL-3.0 WITH Qt-GPL-exception-1.0

#include "requests/getcompletions.h"

#include <texteditor/basehoverhandler.h>

#include <QTextBlock>

#pragma once

namespace TextEditor { class TextSuggestion; }
namespace CodeGeeX2::Internal {

class CodeGeeX2Client;

class CodeGeeX2HoverHandler final : public TextEditor::BaseHoverHandler
{
public:
    CodeGeeX2HoverHandler() = default;

protected:
    void identifyMatch(TextEditor::TextEditorWidget *editorWidget,
                       int pos,
                       ReportPriority report) final;
    void operateTooltip(TextEditor::TextEditorWidget *editorWidget, const QPoint &point) final;

private:
    QTextBlock m_block;
};

} // namespace CodeGeeX2::Internal
