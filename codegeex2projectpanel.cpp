#include "codegeex2projectpanel.h"
#include "codegeex2constants.h"
#include "codegeex2settings.h"

#include <projectexplorer/project.h>
#include <projectexplorer/projectsettingswidget.h>

#include <utils/layoutbuilder.h>

#include <QWidget>

using namespace ProjectExplorer;

namespace CodeGeeX2::Internal {

class CodeGeeX2ProjectSettingsWidget : public ProjectExplorer::ProjectSettingsWidget
{
public:
    CodeGeeX2ProjectSettingsWidget()
    {
        setGlobalSettingsId(Constants::CODEGEEX2_GENERAL_OPTIONS_ID);
        setUseGlobalSettingsCheckBoxVisible(true);
    }
};

ProjectSettingsWidget *createCodeGeeX2ProjectPanel(Project *project)
{
    using namespace Layouting;
    using namespace ProjectExplorer;

    auto widget = new CodeGeeX2ProjectSettingsWidget;
    auto settings = new CodeGeeX2ProjectSettings(project, widget);

    QObject::connect(widget,
                     &ProjectSettingsWidget::useGlobalSettingsChanged,
                     settings,
                     &CodeGeeX2ProjectSettings::setUseGlobalSettings);

    widget->setUseGlobalSettings(settings->useGlobalSettings());
    widget->setEnabled(!settings->useGlobalSettings());

    QObject::connect(widget,
                     &ProjectSettingsWidget::useGlobalSettingsChanged,
                     widget,
                     [widget](bool useGlobal) { widget->setEnabled(!useGlobal); });

    // clang-format off
    Column {
        settings->enableCodeGeeX2,
    }.attachTo(widget);
    // clang-format on

    return widget;
}

} // namespace CodeGeeX2::Internal
