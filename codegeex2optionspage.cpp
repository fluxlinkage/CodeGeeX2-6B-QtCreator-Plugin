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

//        auto warningLabel = new QLabel;
//        warningLabel->setWordWrap(true);
//        warningLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse
//                                              | Qt::LinksAccessibleByKeyboard
//                                              | Qt::TextSelectableByMouse);
//        warningLabel->setText(Tr::tr(
//                                  "Enabling %1 is subject to your agreement and abidance with your applicable "
//                                  "%1 terms. It is your responsibility to know and accept the requirements and "
//                                  "parameters of using tools like %1. This may include, but is not limited to, "
//                                  "ensuring you have the rights to allow %1 access to your code, as well as "
//                                  "understanding any implications of your use of %1 and suggestions produced "
//                                  "(like copyright, accuracy, etc.)." ).arg("CodeGeeX2"));

//        auto authWidget = new AuthWidget();

//        auto helpLabel = new QLabel();
//        helpLabel->setTextFormat(Qt::MarkdownText);
//        helpLabel->setWordWrap(true);
//        helpLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse
//                                           | Qt::LinksAccessibleByKeyboard
//                                           | Qt::TextSelectableByMouse);
//        helpLabel->setOpenExternalLinks(true);
//        connect(helpLabel, &QLabel::linkHovered, [](const QString &link) {
//            QToolTip::showText(QCursor::pos(), link);
//        });

//        // clang-format off
//        helpLabel->setText(Tr::tr(
//                               "The Copilot plugin requires node.js and the Copilot neovim plugin. "
//                               "If you install the neovim plugin as described in %1, "
//                               "the plugin will find the agent.js file automatically.\n\n"
//                               "Otherwise you need to specify the path to the %2 "
//                               "file from the Copilot neovim plugin.",
//                               "Markdown text for the copilot instruction label")
//                               .arg("[README.md](https://github.com/github/copilot.vim)")
//                               .arg("[agent.js](https://github.com/github/copilot.vim/tree/release/copilot/dist)"));

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
//        // clang-format on

//        auto updateAuthWidget = [authWidget]() {
//            authWidget->updateClient(
//                FilePath::fromUserInput(
//                    CodeGeeX2Settings::instance().nodeJsPath.volatileValue().toString()),
//                FilePath::fromUserInput(
//                    CodeGeeX2Settings::instance().distPath.volatileValue().toString()));
//        };

//        connect(CodeGeeX2Settings::instance().nodeJsPath.pathChooser(),
//                &PathChooser::textChanged,
//                authWidget,
//                updateAuthWidget);
//        connect(CodeGeeX2Settings::instance().distPath.pathChooser(),
//                &PathChooser::textChanged,
//                authWidget,
//                updateAuthWidget);
//        updateAuthWidget();

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
