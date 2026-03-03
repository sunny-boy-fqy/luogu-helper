@echo off
REM Bundle script for Windows - expand local includes into single file
REM Usage: bundle.bat [source_file]
REM   source_file: File to bundle (default: main.cpp)

REM Find Python
set PYTHON=
where python >nul 2>&1
if %errorlevel%==0 set PYTHON=python
if not defined PYTHON (
    where py >nul 2>&1
    if %errorlevel%==0 set PYTHON=py
)
if not defined PYTHON (
    echo Error: Python not found! Please install Python or add to PATH.
    exit /b 1
)

setlocal enabledelayedexpansion

REM Get the directory where this script is located
set SCRIPT_DIR=%~dp0

REM Default source file
set SOURCE_FILE=main.cpp
if not "%~1"=="" set SOURCE_FILE=%~1

if not exist "%SOURCE_FILE%" (
    echo Error: %SOURCE_FILE% not found!
    exit /b 1
)

set OUTPUT_FILE=bundled_code.cpp

REM Use Python to do the bundling (cross-platform)
%PYTHON% "%SCRIPT_DIR%bundle.py" "%SOURCE_FILE%"
if %errorlevel% neq 0 (
    echo Error: Bundling failed
    exit /b 1
)

REM Copy to clipboard (Windows)
if exist "%OUTPUT_FILE%" (
    echo Code bundled to %OUTPUT_FILE%
    
    REM Try to copy to clipboard using clip command
    clip < "%OUTPUT_FILE%" 2>nul
    if %errorlevel%==0 (
        echo Copied to clipboard!
    ) else (
        echo Could not copy to clipboard. File saved as %OUTPUT_FILE%
    )
) else (
    echo Error: Bundling failed - no output generated
    exit /b 1
)