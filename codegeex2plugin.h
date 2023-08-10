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
