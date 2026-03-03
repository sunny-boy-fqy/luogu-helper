@echo off
REM Get problem description from Luogu
REM Usage: get_problem.bat

setlocal

REM Get tools directory
set SCRIPT_DIR=%~dp0

REM Check if fetch_problem.py exists
set FETCH_SCRIPT=%SCRIPT_DIR%fetch_problem.py
if not exist "%FETCH_SCRIPT%" (
    REM Search upwards for tools directory
    set DIR=%cd%
    :search_loop
    if exist "%DIR%\tools\fetch_problem.py" (
        set FETCH_SCRIPT=%DIR%\tools\fetch_problem.py
        goto :found
    )
    set PARENT_DIR=
    for %%I in ("%DIR%\..") do set PARENT_DIR=%%~fI
    if "%PARENT_DIR%"=="%DIR%" goto :not_found
    set DIR=%PARENT_DIR%
    goto :search_loop
    :not_found
    echo Error: Could not find tools/fetch_problem.py
    exit /b 1
)

:found
REM Get problem ID from current directory name
for %%I in ("%cd%") do set PROB_ID=%%~nxI

python "%FETCH_SCRIPT%" problem "%PROB_ID%" --target_dir .
if %errorlevel% neq 0 (
    echo Failed to fetch problem description
    exit /b 1
)

echo Successfully saved problem.md