@echo off
REM Bundle script for Windows - expand local includes into single file
REM Usage: bundle.bat [source_file]
REM   source_file: File to bundle (default: main.cpp)

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

REM Check if bundle.exe exists in multiple locations
set BUNDLE_EXE=%SCRIPT_DIR%bundle.exe
if not exist "%BUNDLE_EXE%" (
    set BUNDLE_EXE=%SCRIPT_DIR%..\tools\bundle.exe
)
if not exist "%BUNDLE_EXE%" (
    echo Error: bundle.exe not found!
    echo Searched in:
    echo   - %SCRIPT_DIR%bundle.exe
    echo   - %SCRIPT_DIR%..\tools\bundle.exe
    echo Please ensure you downloaded the complete Windows release package.
    exit /b 1
)

echo Using: %BUNDLE_EXE%

REM Use executable to do the bundling
"%BUNDLE_EXE%" "%SOURCE_FILE%"
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