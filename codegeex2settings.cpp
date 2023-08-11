#include "codegeex2settings.h"

#include <projectexplorer/project.h>
#include "codegeex2constants.h"
#include "codegeex2tr.h"

namespace CodeGeeX2 {

static void initEnableAspect(Utils::BoolAspect &enableCodeGeeX2)
{
    enableCodeGeeX2.setSettingsKey(Constants::ENABLE_CODEGEEX2);
    enableCodeGeeX2.setDisplayName(Tr::tr("Enable CodeGeeX2"));
    enableCodeGeeX2.setLabelText(Tr::tr("Enable CodeGeeX2"));
    enableCodeGeeX2.setToolTip(Tr::tr("Enables the CodeGeeX2 integration."));
    enableCodeGeeX2.setDefaultValue(false);
}

CodeGeeX2Settings::CodeGeeX2Settings()
{
    setAutoApply(false);

    initEnableAspect(enableCodeGeeX2);

    autoComplete.setDisplayName(Tr::tr("Auto Complete"));
    autoComplete.setSettingsKey("CodeGeeX2.Autocomplete");
    autoComplete.setLabelText(Tr::tr("Request completions automatically"));
    autoComplete.setDefaultValue(true);
    autoComplete.setEnabler(&enableCodeGeeX2);
    autoComplete.setToolTip(Tr::tr("Automatically request suggestions for the current text cursor "
                                   "position after changes to the document."));

    url.setDefaultValue("http://127.0.0.1:7860/run/predict");
    url.setDisplayStyle(Utils::StringAspect::LineEditDisplay);
    url.setSettingsKey("CodeGeeX2.URL");
    url.setLabelText(Tr::tr("URL of CodeGeeX2 API:"));
    url.setHistoryCompleter("CodeGeeX2.URL.History");
    url.setDisplayName(Tr::tr("CodeGeeX2 API URL"));
    url.setEnabler(&enableCodeGeeX2);
    url.setToolTip(Tr::tr("Input URL of CodeGeeX2 API."));

    contextLimit.setDefaultValue(8192);
    contextLimit.setRange(100,8192);
    contextLimit.setSettingsKey("CodeGeeX2.ContextLimit");
    contextLimit.setLabelText(Tr::tr("Context length limit:"));
    contextLimit.setDisplayName(Tr::tr("Context length limit"));
    contextLimit.setEnabler(&enableCodeGeeX2);
    contextLimit.setToolTip(Tr::tr("Maximum length of context send to server."));

    length.setDefaultValue(20);
    length.setRange(1,500);
    length.setSettingsKey("CodeGeeX2.Length");
    length.setLabelText(Tr::tr("Output sequence length:"));
    length.setDisplayName(Tr::tr("Output sequence length"));
    length.setEnabler(&enableCodeGeeX2);
    length.setToolTip(Tr::tr("Number of tokens to generate each time."));

    temperarure.setDefaultValue(0.2);
    temperarure.setRange(0.0,1.0);
    temperarure.setSettingsKey("CodeGeeX2.Temperarure");
    temperarure.setLabelText(Tr::tr("Temperature:"));
    temperarure.setDisplayName(Tr::tr("Temperature"));
    temperarure.setEnabler(&enableCodeGeeX2);
    temperarure.setToolTip(Tr::tr("Affects how \"random\" the model’s output is."));

    topK.setDefaultValue(0);
    topK.setRange(0,100);
    topK.setSettingsKey("CodeGeeX2.TopK");
    topK.setLabelText(Tr::tr("Top K:"));
    topK.setDisplayName(Tr::tr("Top K"));
    topK.setEnabler(&enableCodeGeeX2);
    topK.setToolTip(Tr::tr("Affects how \"random\" the model's output is."));

    topP.setDefaultValue(0.95);
    topP.setRange(0.0,1.0);
    topP.setSettingsKey("CodeGeeX2.TopP");
    topP.setLabelText(Tr::tr("Top P:"));
    topP.setDisplayName(Tr::tr("Top P"));
    topP.setEnabler(&enableCodeGeeX2);
    topP.setToolTip(Tr::tr("Affects how \"random\" the model's output is."));

    seed.setDefaultValue(8888);
    seed.setRange(0,65535);
    seed.setSettingsKey("CodeGeeX2.Seed");
    seed.setLabelText(Tr::tr("Seed:"));
    seed.setDisplayName(Tr::tr("Seed"));
    seed.setEnabler(&enableCodeGeeX2);
    seed.setToolTip(Tr::tr("Random number seed."));

    expandHeaders.setDisplayName(Tr::tr("Try to expand headers (experimnetal)"));
    expandHeaders.setSettingsKey("CodeGeeX2.ExpandHeaders");
    expandHeaders.setLabelText(Tr::tr("Try to expand headers (experimnetal)"));
    expandHeaders.setDefaultValue(true);
    expandHeaders.setEnabler(&enableCodeGeeX2);
    expandHeaders.setToolTip(Tr::tr("Try to expand headers when sending requests."));

    braceBalance.setDisplayName(Tr::tr("Brace balance (experimnetal)"));
    braceBalance.setSettingsKey("CodeGeeX2.BraceBalance");
    braceBalance.setLabelText(Tr::tr("Brace balance (experimnetal)"));
    braceBalance.setDefaultValue(true);
    braceBalance.setEnabler(&enableCodeGeeX2);
    braceBalance.setToolTip(Tr::tr("Stop suggestions from breaking brace balance."));
}

CodeGeeX2Settings &CodeGeeX2Settings::instance()
{
    static CodeGeeX2Settings settings;
    return settings;
}

CodeGeeX2ProjectSettings::CodeGeeX2ProjectSettings(ProjectExplorer::Project *project, QObject *parent)
{
    setAutoApply(true);

    useGlobalSettings.setSettingsKey(Constants::CODEGEEX2_USE_GLOBAL_SETTINGS);
    useGlobalSettings.setDefaultValue(true);

    initEnableAspect(enableCodeGeeX2);

    QVariantMap map = project->namedSettings(Constants::CODEGEEX2_PROJECT_SETTINGS_ID).toMap();
    fromMap(map);

    connect(&enableCodeGeeX2, &Utils::BoolAspect::valueChanged, this, [this, project] { save(project); });
    connect(&useGlobalSettings, &Utils::BoolAspect::valueChanged, this, [this, project] { save(project); });

}

void CodeGeeX2ProjectSettings::save(ProjectExplorer::Project *project)
{
    QVariantMap map;
    toMap(map);
    project->setNamedSettings(Constants::CODEGEEX2_PROJECT_SETTINGS_ID, map);

    // This triggers a restart of the Copilot language server.
    CodeGeeX2Settings::instance().apply();
}

void CodeGeeX2ProjectSettings::setUseGlobalSettings(bool useGlobal)
{
    useGlobalSettings.setValue(useGlobal);
}

bool CodeGeeX2ProjectSettings::isEnabled() const
{
    if (useGlobalSettings())
        return CodeGeeX2Settings::instance().enableCodeGeeX2();
    return enableCodeGeeX2();
}

} // namespace CodeGeeX2
