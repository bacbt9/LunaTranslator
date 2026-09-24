# Building LunaHook

LunaHook is Windows-only and needs MSVC. It produces:

| file | what it is |
| --- | --- |
| `LunaHook32.dll` / `LunaHook64.dll` | the hook that gets injected into the game |
| `LunaHost32.dll` / `LunaHost64.dll` | host library (what LunaTranslator loads) |
| `LunaHost32.exe` / `LunaHost64.exe` | standalone GUI host (Textractor-style) |
| `LunaTmpFontLoader.dll` | helper used by the Unity engine hook |

Use the 32-bit host for 32-bit games and the 64-bit host for 64-bit games.

## Option 1: GitHub Actions (no local setup)

1. Open the **Actions** tab → **buildlunahook** → **Run workflow**.
   (It also runs automatically on every push that touches LunaHook.)
2. When it finishes, download `LunaHook_win10`, `LunaHook_win7` or
   `LunaHook_winxp` from the run's **Artifacts** section.
3. Tick **"Also publish the zips to the 'LunaHook' GitHub release"** if you
   want them attached to a release instead.

## Option 2: build locally

Install once:

- [Visual Studio 2022 or 2026](https://visualstudio.microsoft.com/) (Community is fine)
  with the **Desktop development with C++** workload
- [Python 3.8+](https://www.python.org/)
- [Git](https://git-scm.com/) (CMake uses it to download minhook/uchardet)

Then, from a normal terminal in the repo root:

```bat
python src\scripts\build_lunahook.py
```

The script finds Visual Studio itself (it uses the CMake bundled with VS if
`cmake` isn't on PATH — or run it from the "Developer Command Prompt" to be
sure). Output ends up in `src\NativeImpl\LunaHook\builds\Release_win10\` and
`src\NativeImpl\LunaHook\builds\LunaHook_win10.zip`.

Useful flags:

```bat
python src\scripts\build_lunahook.py --target win7          # runs on Windows 7+
python src\scripts\build_lunahook.py --target winxp --arch x86
python src\scripts\build_lunahook.py --arch x64             # only 64-bit
python src\scripts\build_lunahook.py --no-gui               # only the DLLs
python src\scripts\build_lunahook.py --clean                # wipe old build dirs
python src\scripts\build_lunahook.py --generator "Visual Studio 17 2022"
```

`win10` builds the host with the dynamic MSVC runtime, so it needs the
[VC++ redistributable](https://aka.ms/vs/17/release/vc_redist.x64.exe) (most
PCs already have it). `win7`/`winxp` builds are fully static.
