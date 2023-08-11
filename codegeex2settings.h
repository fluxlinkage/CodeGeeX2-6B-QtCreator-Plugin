#ifndef CODEGEEX2_CODEGEEX2SETTINGS_H
#define CODEGEEX2_CODEGEEX2SETTINGS_H

#include <utils/aspects.h>

namespace ProjectExplorer {
class Project;
}

namespace CodeGeeX2 {

class CodeGeeX2Settings : public Utils::AspectContainer
{
public:
    CodeGeeX2Settings();

    static CodeGeeX2Settings &instance();

    Utils::BoolAspect enableCodeGeeX2{this};
    Utils::BoolAspect autoComplete{this};
    Utils::StringAspect url{this};
    Utils::IntegerAspect contextLimit{this};
    Utils::IntegerAspect length{this};
    Utils::DoubleAspect temperarure{this};
    Utils::IntegerAspect topK{this};
    Utils::DoubleAspect topP{this};
    Utils::IntegerAspect seed{this};
    Utils::BoolAspect expandHeaders{this};
    Utils::BoolAspect braceBalance{this};
};

class CodeGeeX2ProjectSettings : public Utils::AspectContainer
{
public:
    CodeGeeX2ProjectSettings(ProjectExplorer::Project *project, QObject *parent = nullptr);

    void save(ProjectExplorer::Project *project);
    void setUseGlobalSettings(bool useGlobal);

    bool isEnabled() const;

    Utils::BoolAspect enableCodeGeeX2{this};
    Utils::BoolAspect useGlobalSettings{this};
};

} // namespace CodeGeeX2

#endif // CODEGEEX2_CODEGEEX2SETTINGS_H
