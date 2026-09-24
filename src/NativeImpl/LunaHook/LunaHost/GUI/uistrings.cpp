#include "InfoStrings.h"
#include <unordered_map>
#include <string>

// UI strings for the standalone LunaHost GUI.
// (The per-language tables that used to live in lang_ui were removed upstream.)
static std::unordered_map<LANG_STRINGS_UI, i18nString<wchar_t>> _internal_lang_strings_ui = {
    {WndSelectProcess, L"Select Process"},
    {WndLunaHostGui, L"LunaHost"},
    {TSetting, L"Settings"},
    {TPlugins, L"Plugins"},
    {NotifyInvalidHookCode, L"Invalid hook code"},
    {BtnDetach, L"Detach All"},
    {BtnSaveHook, L"Save Hook"},
    {BtnAttach, L"Attach"},
    {BtnRefresh, L"Refresh"},
    {BtnToClipboard, L"Copy selected text to clipboard"},
    {BtnReadOnly, L"Read only"},
    {BtnInsertUserHook, L"Insert Hook Code"},
    {LblFlushDelay, L"Flush delay (ms)"},
    {LblFilterRepeat, L"Filter repetition"},
    {LblCodePage, L"Default code page (0 = auto)"},
    {LblMaxBuff, L"Max buffer size"},
    {LblMaxHist, L"Max history size"},
    {LblLanguage, L"Language"},
    {LblAutoAttach, L"Auto attach"},
    {LblAutoAttach_savedonly, L"Auto attach (saved hooks only)"},
    {MenuCopyHookCode, L"Copy hook code"},
    {MenuRemoveHook, L"Remove hook"},
    {MenuDetachProcess, L"Detach process"},
    {MenuRemeberSelect, L"Remember this hook"},
    {MenuForgetSelect, L"Forget remembered hook"},
    {MenuAddPlugin, L"Add plugin"},
    {MenuRemovePlugin, L"Remove plugin"},
    {MenuPluginRankUp, L"Move up"},
    {MenuPluginRankDown, L"Move down"},
    {MenuPluginEnable, L"Enabled"},
    {MenuPluginVisSetting, L"Show settings window"},
    {DefaultFont, L"Microsoft YaHei"},
    {InvalidPlugin, L"Not a valid plugin"},
    {InvalidDll, L"Could not load DLL"},
    {InvalidDump, L"Plugin already added"},
    {MsgError, L"Error"},
    {BtnOk, L"OK"},
    {HS_TEXT, L"Text"},
    {VersionLatest, L"Latest"},
    {VersionCurrent, L"Version"},
    {LIST_HOOK, L"Hook"},
    {COPYSELECTION, L"Copy selection"},
    {FONTSELECT, L"Select font"},
};

const wchar_t *langhelper::operator[](LANG_STRINGS_UI langstring)
{
    return _internal_lang_strings_ui[langstring].get();
}
std::unordered_map<LANG_STRINGS_UI, i18nString<wchar_t>> &langhelper::get_ui()
{
    return _internal_lang_strings_ui;
}
