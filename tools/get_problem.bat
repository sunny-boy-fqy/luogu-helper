@echo off
REM Get problem description from Luogu
REM Usage: get_problem.bat

setlocal

REM Get script directory (use %~dp0 directly)
set SCRIPT_DIR=%~dp0

REM Check if fetch_problem.exe exists
set FETCH_EXE=%SCRIPT_DIR%fetch_problem.exe
if not exist "%FETCH_EXE%" (
    echo Error: fetch_problem.exe not found!
    echo Please ensure you downloaded the complete Windows release package.
    exit /b 1
)

REM Get problem ID from current directory name
for %%I in ("%cd%") do set PROB_ID=%%~nxI

echo Fetching problem %PROB_ID%...
"%FETCH_EXE%" problem "%PROB_ID%" --target_dir .
if %errorlevel% neq 0 (
    echo Failed to fetch problem description
    exit /b 1
)

echo Successfully saved problem.md