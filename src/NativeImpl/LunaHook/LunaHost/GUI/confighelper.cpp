#include <io.h>
#include "confighelper.h"
#include "stringutils.h"
std::string readfile(const wchar_t *fname)
{
    FILE *f;
    _wfopen_s(&f, fname, L"rb");
    if (f == 0)
        return {};
    fseek(f, 0, SEEK_END);
    auto len = ftell(f);
    fseek(f, 0, SEEK_SET);
    std::string buff;
    buff.resize(len);
    fread(buff.data(), 1, len, f);
    fclose(f);
    return buff;
}
void writefile(const wchar_t *fname, const std::string &s)
{
    // write to a temp file and swap it in, so a crash/power loss can't leave a truncated config
    std::wstring tmp = std::wstring(fname) + L".tmp";
    FILE *f;
    _wfopen_s(&f, tmp.c_str(), L"wb");
    if (!f)
        return;
    fwrite(s.data(), 1, s.size(), f);
    fflush(f);
    _commit(_fileno(f));
    fclose(f);
    MoveFileExW(tmp.c_str(), fname, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
}

confighelper::confighelper()
{
    // keep config.json next to the exe, not in whatever the working directory happens to be
    configpath = std::filesystem::path(getModuleFilename().value()).replace_filename(L"config.json");
    auto content = readfile(configpath.c_str());
    if (content.empty()) // older builds saved to the working directory
        content = readfile((std::filesystem::current_path() / "config.json").c_str());
    try
    {
        configs = nlohmann::json::parse(content);
    }
    catch (std::exception &)
    {
        configs = {};
    }

    if (configs.find(pluginkey) == configs.end())
    {
        configs[pluginkey] = {};
    }
}
void confighelper::save()
{
    writefile(configpath.c_str(), configs.dump(4));
}
confighelper::~confighelper()
{
    save();
}