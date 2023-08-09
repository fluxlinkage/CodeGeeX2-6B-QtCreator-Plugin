// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#pragma once

namespace ProjectExplorer {
class ProjectSettingsWidget;
class Project;
} // namespace ProjectExplorer

namespace CodeGeeX2::Internal {

ProjectExplorer::ProjectSettingsWidget *createCodeGeeX2ProjectPanel(ProjectExplorer::Project *project);

} // namespace CodeGeeX2::Internal
