#pragma once

#include "codegeex2hoverhandler.h"
#include "requests/getcompletions.h"

#include <languageclient/client.h>

#include <utils/filepath.h>

#include <QHash>
#include <QTemporaryDir>

namespace CodeGeeX2::Internal {

class CodeGeeX2Client : public LanguageClient::Client
{
public:
    CodeGeeX2Client();
    ~CodeGeeX2Client() override;

    void openDocument(TextEditor::TextDocument *document) override;

    void scheduleRequest(TextEditor::TextEditorWidget *editor);
    void requestCompletions(TextEditor::TextEditorWidget *editor);
    void handleCompletions(const GetCompletionRequest::Response &response,
                           TextEditor::TextEditorWidget *editor);
    void cancelRunningRequest(TextEditor::TextEditorWidget *editor);

    bool canOpenProject(ProjectExplorer::Project *project) override;

    bool isEnabled(ProjectExplorer::Project *project);

private:
    QMap<TextEditor::TextEditorWidget *, GetCompletionRequest> m_runningRequests;
    struct ScheduleData
    {
        int cursorPosition = -1;
        QTimer *timer = nullptr;
    };
    QMap<TextEditor::TextEditorWidget *, ScheduleData> m_scheduledRequests;
    CodeGeeX2HoverHandler m_hoverHandler;
};

} // namespace CodeGeeX2::Internal
