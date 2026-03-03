#!/usr/bin/env python3
# build_executables.py - Build standalone executables for Windows and Linux
import os
import sys
import subprocess
import platform


def build_executable(script_path, output_name):
    """Build standalone executable using PyInstaller with hidden imports"""
    cmd = [
        "pyinstaller",
        "--onefile",
        "--name",
        output_name,
        "--distpath",
        "dist",
        "--workpath",
        "build",
        "--specpath",
        "build",
        "--clean",
        "--hidden-import",
        "requests",
        "--hidden-import",
        "json",
        "--hidden-import",
        "re",
        script_path,
    ]

    # Add console flag for Windows (keep console for command-line tools)
    if platform.system() == "Windows":
        cmd.append("--console")

    print(f"Building {output_name} for {platform.system()}...")
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error building {output_name}:", result.stderr)
        return False

    # Verify the executable was created
    expected_path = f"dist/{output_name}"
    if platform.system() == "Windows" and not output_name.endswith(".exe"):
        expected_path += ".exe"

    if not os.path.exists(expected_path):
        print(f"Error: Expected executable not found at {expected_path}")
        return False

    print(f"Successfully built {output_name}")
    return True


if __name__ == "__main__":
    # Check if pyinstaller is installed
    try:
        import PyInstaller
    except ImportError:
        print("Installing PyInstaller and requests...")
        subprocess.run(
            [sys.executable, "-m", "pip", "install", "pyinstaller", "requests"]
        )

    scripts = [
        ("tools/fetch_problem.py", "fetch_problem"),
        ("tools/bundle.py", "bundle"),
    ]

    success = True
    for script_path, output_name in scripts:
        if not build_executable(script_path, output_name):
            success = False

    if success:
        print("All executables built successfully!")
        print("Built executables:")
        for f in os.listdir("dist"):
            print(f"  - dist/{f}")
    else:
        print("Some executables failed to build!")
        sys.exit(1)
