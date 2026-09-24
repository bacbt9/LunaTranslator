"""
Standalone LunaHook build.

Builds LunaHook{32,64}.dll (the injected hook), LunaHost{32,64}.dll (the host
library) and, when possible, the LunaHost{32,64}.exe GUI, then packs them into
src/NativeImpl/LunaHook/builds/LunaHook_<target>.zip.

Requirements (Windows):
  - Visual Studio 2022 or 2026 with "Desktop development with C++"
  - CMake (the one bundled with Visual Studio is fine) and git on PATH
  - Python 3.8+

Usage:
  python src/scripts/build_lunahook.py                    # win10, x86+x64, with GUI
  python src/scripts/build_lunahook.py --target win7
  python src/scripts/build_lunahook.py --target winxp --arch x86
  python src/scripts/build_lunahook.py --no-gui
  python src/scripts/build_lunahook.py --generator "Visual Studio 17 2022"

Targets:
  win10  dynamic CRT for the host, needs Windows 10+
  win7   static CRT (VC-LTL) + YY-Thunks, runs on Windows 7+
  winxp  like win7 but thunked down to Windows XP (x86 only is useful)
"""

import argparse
import os
import shutil
import subprocess
import sys
import zipfile

SCRIPTDIR = os.path.dirname(os.path.abspath(__file__))
LUNAHOOK = os.path.abspath(os.path.join(SCRIPTDIR, "../NativeImpl/LunaHook"))
BUILDS = os.path.join(LUNAHOOK, "builds")


def run(cmd, check=True):
    print(">>", " ".join(cmd), flush=True)
    r = subprocess.run(cmd, cwd=LUNAHOOK)
    if check and r.returncode != 0:
        sys.exit(f"command failed with exit code {r.returncode}")
    return r.returncode == 0


def detect_generator():
    vswhere = os.path.join(
        os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)"),
        r"Microsoft Visual Studio\Installer\vswhere.exe",
    )
    if os.path.exists(vswhere):
        out = subprocess.run(
            [vswhere, "-latest", "-products", "*", "-property", "installationVersion"],
            capture_output=True,
            text=True,
        ).stdout.strip()
        major = out.split(".")[0] if out else ""
        known = {"18": "Visual Studio 18 2026", "17": "Visual Studio 17 2022"}
        if major in known:
            return known[major]
    sys.exit(
        "Could not detect a Visual Studio installation (2022 or 2026 needed). "
        'Pass --generator "Visual Studio 17 2022" explicitly.'
    )


def target_flags(target):
    return {
        "win10": ["-DWIN10ABOVE=ON"],
        "win7": ["-DWIN10ABOVE=OFF"],
        "winxp": ["-DWINXP=ON", "-DWIN10ABOVE=OFF"],
    }[target]


def configure(builddir, generator, arch, flags):
    platform = "x64" if arch == "x64" else "Win32"
    run(
        [
            "cmake",
            "-S", ".",
            "-B", builddir,
            "-G", generator,
            "-A", platform,
            "-T", f"host={arch}",
            *flags,
        ]
    )


def build(builddir, target=None, check=True):
    cmd = ["cmake", "--build", builddir, "--config", "Release", "-j", str(os.cpu_count())]
    if target:
        cmd += ["--target", target]
    return run(cmd, check=check)


def patch_pe_for_xp(outdir):
    # Same trick the LunaTranslator build uses: compile with the normal toolset
    # (+ YY-Thunks/VC-LTL), then lower the PE subsystem version to 5.1.
    try:
        import pefile
    except ImportError:
        run([sys.executable, "-m", "pip", "install", "pefile"])
        import pefile
    for name in os.listdir(outdir):
        path = os.path.join(outdir, name)
        if os.path.splitext(name)[1].lower() not in (".dll", ".exe"):
            continue
        pe = pefile.PE(path)
        oh = pe.OPTIONAL_HEADER
        oh.MajorOperatingSystemVersion = 5
        oh.MinorOperatingSystemVersion = 1
        oh.MajorSubsystemVersion = 5
        oh.MinorSubsystemVersion = 1
        data = pe.write()
        pe.close()
        with open(path, "wb") as f:
            f.write(data)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--target", choices=["win10", "win7", "winxp"], default="win10")
    ap.add_argument("--arch", choices=["x86", "x64", "all"], default="all")
    ap.add_argument("--no-gui", action="store_true", help="skip the LunaHost.exe GUI")
    ap.add_argument(
        "--require-gui",
        action="store_true",
        help="fail if the GUI does not build (by default a GUI failure is only a warning)",
    )
    ap.add_argument("--generator", help='e.g. "Visual Studio 17 2022" (auto-detected by default)')
    ap.add_argument("--clean", action="store_true", help="delete old build dirs first")
    ap.add_argument("--no-zip", action="store_true")
    args = ap.parse_args()

    if os.name != "nt":
        sys.exit("LunaHook can only be built on Windows (MSVC).")

    generator = args.generator or detect_generator()
    archs = ["x86", "x64"] if args.arch == "all" else [args.arch]
    target = args.target
    # must match binary_out_putpath in LunaHook/CMakeLists.txt
    outdir = os.path.join(BUILDS, f"Release_{target}")

    print("generator:", generator)
    print("target:   ", target)
    print("archs:    ", archs)
    print("output:   ", outdir, flush=True)

    if args.clean:
        shutil.rmtree(os.path.join(LUNAHOOK, "build"), ignore_errors=True)
    shutil.rmtree(outdir, ignore_errors=True)

    gui_failed = []
    for arch in archs:
        flags = target_flags(target)

        # 1) the hook dll that gets injected into the game
        hookdir = f"build/{arch}_{target}_hook"
        configure(hookdir, generator, arch, flags + ["-DBUILD_HOST=OFF"])
        build(hookdir)

        # 2) the host dll (+ optional GUI exe)
        hostflags = flags + ["-DBUILD_HOOK=OFF", f"-DBUILD_GUI={'OFF' if args.no_gui else 'ON'}"]
        if target != "win10":
            hostflags.append("-DUSE_VC_LTL=ON")
        hostdir = f"build/{arch}_{target}_host"
        configure(hostdir, generator, arch, hostflags)
        build(hostdir, "LunaHostDll")
        if not args.no_gui:
            if not build(hostdir, "LunaHost", check=args.require_gui):
                gui_failed.append(arch)

    # keep only the binaries
    for name in os.listdir(outdir):
        if os.path.splitext(name)[1].lower() not in (".dll", ".exe"):
            p = os.path.join(outdir, name)
            shutil.rmtree(p) if os.path.isdir(p) else os.remove(p)
    if target == "winxp":
        patch_pe_for_xp(outdir)

    print("\nbuilt files:")
    for name in sorted(os.listdir(outdir)):
        print("  ", name)

    if not args.no_zip:
        zippath = os.path.join(BUILDS, f"LunaHook_{target}.zip")
        with zipfile.ZipFile(zippath, "w", zipfile.ZIP_DEFLATED, compresslevel=9) as z:
            for name in sorted(os.listdir(outdir)):
                z.write(os.path.join(outdir, name), f"LunaHook_{target}/{name}")
        print("zip:", zippath)

    if gui_failed:
        print(f"\nWARNING: GUI (LunaHost.exe) failed to build for {gui_failed}; DLLs are fine.")


if __name__ == "__main__":
    main()
