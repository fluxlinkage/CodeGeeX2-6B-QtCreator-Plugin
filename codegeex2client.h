// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0+ OR GPL-3.0 WITH Qt-GPL-exception-1.0

#pragma once

#include "codegeex2hoverhandler.h"
//#include "requests/checkstatus.h"
#include "requests/getcompletions.h"
//#include "requests/signinconfirm.h"
//#include "requests/signininitiate.h"
//#include "requests/signout.h"

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

//    void requestCheckStatus(
//        bool localChecksOnly,
//        std::function<void(const CheckStatusRequest::Response &response)> callback);

//    void requestSignOut(std::function<void(const SignOutRequest::Response &response)> callback);

//    void requestSignInInitiate(
//        std::function<void(const SignInInitiateRequest::Response &response)> callback);

//    void requestSignInConfirm(
//        const QString &userCode,
//        std::function<void(const SignInConfirmRequest::Response &response)> callback);

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
