#include "InfoStrings.h"
#include <unordered_map>
#include <string>
#include <optional>

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
    {DefaultFont, L"Meiryo UI"},
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

// English for the host/hook console messages (their built-in text is Chinese). The host asks
// through its i18n callback, keyed by the original string; keys copied from include/InfoStrings.cpp.
std::optional<std::wstring> englishTranslation(const std::wstring &text)
{
    static const std::unordered_map<std::wstring, std::wstring> table = {
        {L"警告", L"Warning"},
        {L"已经注入", L"Already injected"},
        {L"注入失败", L"Injection failed"},
        {L"无法转换文本 (无效的代码页?)", L"Can't convert text (invalid code page?)"},
        {L"进程 %d 已连接", L"Process %d attached"},
        {L"进程 %d 已断开连接", L"Process %d detached"},
        {L"文件版本无法匹配，可能无法正常工作，请重新下载！", L"File versions don't match, this may not work correctly. Please re-download!"},
        {L"注入钩子: %s %p", L"Inserting hook: %s %p"},
        {L"移除钩子: %s", L"Removing hook: %s"},
        {L"钩子数量已达上限: 无法注入", L"Too many hooks: can't insert"},
        {L"开始搜索钩子", L"Starting hook search"},
        {L"初始化钩子搜索 (%f%%)", L"Initializing hook search (%f%%)"},
        {L"文本长度不足, 无法精确搜索", L"Not enough text to search accurately"},
        {L"搜索初始化完成, 创建了 %zd 个钩子", L"Search initialized with %zd hooks"},
        {L"请点击游戏区域, 在接下来的 %d 秒内使游戏强制处理文本", L"Click on the game and make it show text within the next %d seconds"},
        {L"钩子搜索完毕, 找到了 %d 条结果", L"Hook search finished, %d results found"},
        {L"搜索结果已达上限, 如果结果不理想, 请重试(默认最大记录数增加)", L"Out of search records; if the results are poor, try again (default max records increased)"},
        {L"函数不存在", L"Function not present"},
        {L"模块不存在", L"Module not present"},
        {L"内存一直在改变，无法有效读取", L"Memory keeps changing, can't read it reliably"},
        {L"Sender 错误 (可能是由于错误或不稳定的 H-code) ： %s", L"Sender error (probably an incorrect or unstable H-code): %s"},
        {L"Reader 错误 (可能是由于错误或不稳定的 R-code) ： %s", L"Reader error (probably an incorrect or unstable R-code): %s"},
        {L"搜索钩子错误 : 内存溢出，尝试重新分配 %d", L"Hook search error: out of memory, retrying with %d"},
        {L"%d 个结果被找到", L"%d results found"},
        {L"无法找到文本", L"Could not find text"},
        {L"可能存在错误 (无效的文本长度 %d 出现在 %s)", L"Possible error (invalid text length %d at %s)"},
        {L"钩子注入失败 %s", L"Failed to insert hook %s"},
        {L"匹配 %s 引擎时发生错误", L"Error while matching engine %s"},
        {L"连接到 %s 引擎时发生错误", L"Error while attaching to engine %s"},
        {L"匹配到 %s 引擎", L"Matched engine: %s"},
        {L"确认是 %s 引擎", L"Confirmed engine: %s"},
        {L"成功连接到 %s 引擎", L"Attached to engine: %s"},
        {L"获取到进程内存地址范围 0x%p 到 0x%p", L"Process memory range: 0x%p to 0x%p"},
        {L"警告，注入的进程内存很小，可能是无用进程!", L"Warning: the injected process is very small, it may be the wrong process!"},
        {L"不支持ryujinx，请使用yuzu/sudachi/Citron/Eden", L"Ryujinx is not supported, please use yuzu/sudachi/Citron/Eden"},
        {L"不支持当前模拟器版本", L"This emulator version is not supported"},
        {L"特殊码无效", L"Invalid hook code"},
    };
    auto it = table.find(text);
    if (it == table.end())
        return std::nullopt;
    return it->second;
}
