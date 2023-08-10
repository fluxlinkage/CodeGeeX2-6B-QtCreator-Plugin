#include "codegeex2optionspage.h"

#include <coreplugin/icore.h>
#include <utils/layoutbuilder.h>
#include "codegeex2constants.h"
#include "codegeex2settings.h"

namespace CodeGeeX2 {

class CodeGeeX2OptionsPageWidget : public Core::IOptionsPageWidget
{
public:
    CodeGeeX2OptionsPageWidget()
    {
        using namespace Layouting;

        Column {
            CodeGeeX2Settings::instance().enableCodeGeeX2, br,
            CodeGeeX2Settings::instance().autoComplete, br,
            CodeGeeX2Settings::instance().url, br,
            CodeGeeX2Settings::instance().contextLimit, br,
            CodeGeeX2Settings::instance().length, br,
            CodeGeeX2Settings::instance().temperarure, br,
            CodeGeeX2Settings::instance().topK, br,
            CodeGeeX2Settings::instance().topP, br,
            CodeGeeX2Settings::instance().seed, br,
            st
        }.attachTo(this);

        setOnApply([] {
            CodeGeeX2Settings::instance().apply();
            CodeGeeX2Settings::instance().writeSettings(Core::ICore::settings());
        });
    }
};

CodeGeeX2OptionsPage::CodeGeeX2OptionsPage()
{
    setId(Constants::CODEGEEX2_GENERAL_OPTIONS_ID);
    setDisplayName("CodeGeeX2");
    setCategory(Constants::CODEGEEX2_GENERAL_OPTIONS_CATEGORY);
    setDisplayCategory(Constants::CODEGEEX2_GENERAL_OPTIONS_DISPLAY_CATEGORY);
    setCategoryIconPath(":/codegeex2/images/settingscategory_codegeex2.png");
    setWidgetCreator([] { return new CodeGeeX2OptionsPageWidget; });
}

CodeGeeX2OptionsPage &CodeGeeX2OptionsPage::instance()
{
    static CodeGeeX2OptionsPage settingsPage;
    return settingsPage;
}

} // namespace CodeGeeX2
