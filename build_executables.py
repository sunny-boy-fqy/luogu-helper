#!/usr/bin/env python3
# build_executables.py - Build standalone executables for Windows
import os
import sys
import subprocess


def build_executable(script_path, output_name):
    """Build standalone executable using PyInstaller"""
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
        script_path,
    ]

    print(f"Building {output_name}...")
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error building {output_name}:", result.stderr)
        return False
    print(f"Successfully built {output_name}")
    return True


if __name__ == "__main__":
    # Check if pyinstaller is installed
    try:
        import PyInstaller
    except ImportError:
        print("Installing PyInstaller...")
        subprocess.run([sys.executable, "-m", "pip", "install", "pyinstaller"])

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
    else:
        print("Some executables failed to build!")
        sys.exit(1)
