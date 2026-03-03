@echo off
REM Get problem description from Luogu
REM Usage: get_problem.bat

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

setlocal

REM Get script directory
set SCRIPT_DIR=%~dp0
set SCRIPT_DIR=%SCRIPT_DIR:~0,-1%
for %%i in ("%SCRIPT_DIR%") do set TOOLS_DIR=%%~dpi
set TOOLS_DIR=%TOOLS_DIR:~0,-1%

REM Check if fetch_problem.py exists
set FETCH_SCRIPT=%TOOLS_DIR%\tools\fetch_problem.py
if not exist "%FETCH_SCRIPT%" (
    echo Error: Could not find fetch_problem.py
    exit /b 1
)

:found
REM Get problem ID from current directory name
for %%I in ("%cd%") do set PROB_ID=%%~nxI

echo Fetching problem %PROB_ID%...
%PYTHON% "%FETCH_SCRIPT%" problem "%PROB_ID%" --target_dir .
if %errorlevel% neq 0 (
    echo Failed to fetch problem description
    exit /b 1
)

echo Successfully saved problem.md