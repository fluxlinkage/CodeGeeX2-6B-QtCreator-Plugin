// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#pragma once

#include "codegeex2client.h"

#include <extensionsystem/iplugin.h>

#include <QPointer>

namespace CodeGeeX2 {
namespace Internal {

class CodeGeeX2Plugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "CodeGeeX2.json")

public:
    void initialize() override;
    void extensionsInitialized() override;
    void restartClient();
    ShutdownFlag aboutToShutdown() override;

private:
    QPointer<CodeGeeX2Client> m_client;
};

} // namespace Internal
} // namespace CodeGeeX2
