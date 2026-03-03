@echo off
REM Get solution from Luogu
REM Usage: get_solve.bat

setlocal

REM Get script directory (use %~dp0 directly)
set SCRIPT_DIR=%~dp0

REM Check if fetch_problem.exe exists in multiple locations
set FETCH_EXE=%SCRIPT_DIR%fetch_problem.exe
if not exist "%FETCH_EXE%" (
    set FETCH_EXE=%SCRIPT_DIR%..\tools\fetch_problem.exe
)
if not exist "%FETCH_EXE%" (
    echo Error: fetch_problem.exe not found!
    echo Searched in:
    echo   - %SCRIPT_DIR%fetch_problem.exe
    echo   - %SCRIPT_DIR%..\tools\fetch_problem.exe
    echo Please ensure you downloaded the complete Windows release package.
    exit /b 1
)

echo Using: %FETCH_EXE%

REM Get problem ID from current directory name
for %%I in ("%cd%") do set PROB_ID=%%~nxI

echo Fetching solution for %PROB_ID%...
"%FETCH_EXE%" solve "%PROB_ID%" --target_dir .
if %errorlevel% neq 0 (
    echo Failed to fetch solution
    exit /b 1
)

echo Successfully saved std.cpp