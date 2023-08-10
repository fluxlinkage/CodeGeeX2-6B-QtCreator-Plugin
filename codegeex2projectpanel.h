#pragma once

namespace ProjectExplorer {
class ProjectSettingsWidget;
class Project;
} // namespace ProjectExplorer

namespace CodeGeeX2::Internal {

ProjectExplorer::ProjectSettingsWidget *createCodeGeeX2ProjectPanel(ProjectExplorer::Project *project);

} // namespace CodeGeeX2::Internal
