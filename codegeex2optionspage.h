#ifndef CODEGEEX2OPTIONSPAGE_H
#define CODEGEEX2OPTIONSPAGE_H

#include <coreplugin/dialogs/ioptionspage.h>

namespace CodeGeeX2 {

class CodeGeeX2OptionsPage : public Core::IOptionsPage
{
public:
    CodeGeeX2OptionsPage();

    static CodeGeeX2OptionsPage &instance();
};
} // namespace CodeGeeX2

#endif // CODEGEEX2OPTIONSPAGE_H
