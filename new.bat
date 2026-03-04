@echo off
setlocal enabledelayedexpansion
REM OI Problem setup script for Windows
REM Usage: new.bat ^<PROBLEM_ID_OR_FOLDER_NAME^>

if "%~1"=="" (
    echo Usage: new.bat ^<PROBLEM_ID_OR_FOLDER_NAME^>
    exit /b 1
)

set TARGET=%~1
set PROB_ID=%TARGET%

REM Get script directory (use %~dp0 directly)
set SCRIPT_DIR=%~dp0
set STD_DIR=%SCRIPT_DIR%std

REM Check if it's a problem ID
REM Windows findstr doesn't support $ for end-of-line, so we use different logic
set IS_PROBLEM_ID=0

REM Convert to uppercase for matching
set PROB_ID_UPPER=%PROB_ID%

REM Check for P/B/T/U followed by digits (e.g., P1001, B2001, T123, U99999)
set "FIRST_CHAR=!PROB_ID_UPPER:~0,1!"
set "REST=!PROB_ID_UPPER:~1!"
if "!FIRST_CHAR!"=="P" (
    if not "!REST!"=="" (
        echo !REST! | findstr /R "^[0-9][0-9]*$" >nul 2>&1
        if !errorlevel!==0 set IS_PROBLEM_ID=1
    )
)
if "!FIRST_CHAR!"=="B" (
    if not "!REST!"=="" (
        echo !REST! | findstr /R "^[0-9][0-9]*$" >nul 2>&1
        if !errorlevel!==0 set IS_PROBLEM_ID=1
    )
)
if "!FIRST_CHAR!"=="T" (
    if not "!REST!"=="" (
        echo !REST! | findstr /R "^[0-9][0-9]*$" >nul 2>&1
        if !errorlevel!==0 set IS_PROBLEM_ID=1
    )
)
if "!FIRST_CHAR!"=="U" (
    if not "!REST!"=="" (
        echo !REST! | findstr /R "^[0-9][0-9]*$" >nul 2>&1
        if !errorlevel!==0 set IS_PROBLEM_ID=1
    )
)

REM Check for digits followed by single uppercase letter (e.g., 1234A)
echo !PROB_ID_UPPER! | findstr /R "^[0-9][0-9]*[A-Z]$" >nul 2>&1
if !errorlevel!==0 set IS_PROBLEM_ID=1

REM Check for underscore in name
echo %TARGET% | findstr "_" >nul 2>&1
if !errorlevel!==0 set IS_PROBLEM_ID=1

if !IS_PROBLEM_ID!==1 goto :is_problem_id

REM Not a problem ID
echo Creating directory: %TARGET%
if not exist "%STD_DIR%" (
    echo Error: std directory not found at %STD_DIR%
    exit /b 1
)

REM Create target directory first
mkdir "%SCRIPT_DIR%%TARGET%" 2>nul
xcopy /E /I /Q "%STD_DIR%" "%SCRIPT_DIR%%TARGET%\" >nul 2>&1
echo Successfully set up %TARGET%
echo Please run: cd "%SCRIPT_DIR%%TARGET%"
exit /b 0

:is_problem_id
echo Detected problem ID: %TARGET%

if exist "%SCRIPT_DIR%%TARGET%" (
    echo Error: Directory %TARGET% already exists.
    exit /b 1
)

if not exist "%STD_DIR%" (
    echo Error: std directory not found at %STD_DIR%
    exit /b 1
)

REM Create target directory first
mkdir "%SCRIPT_DIR%%TARGET%" 2>nul
xcopy /E /I /Q "%STD_DIR%" "%SCRIPT_DIR%%TARGET%\" >nul 2>&1

REM Check if fetch_problem.exe exists in multiple locations
set FETCH_EXE=%SCRIPT_DIR%tools\fetch_problem.exe
if not exist "%FETCH_EXE%" (
    set FETCH_EXE=%SCRIPT_DIR%fetch_problem.exe
)
if not exist "%FETCH_EXE%" (
    echo Error: fetch_problem.exe not found!
    echo Searched in:
    echo   - %SCRIPT_DIR%tools\fetch_problem.exe
    echo   - %SCRIPT_DIR%fetch_problem.exe
    echo Please ensure you downloaded the complete Windows release package.
    exit /b 1
)

echo Using: %FETCH_EXE%

echo Fetching samples from Luogu...
"%FETCH_EXE%" samples "%TARGET%" --target_dir "%SCRIPT_DIR%%TARGET%"

if %errorlevel%==0 (
    echo Successfully set up %PROB_ID%
    echo Please run: cd "%SCRIPT_DIR%%TARGET%"
    exit /b 0
) else (
    echo Failed to fetch samples, but directory created.
    echo You may need to configure LUOGU_COOKIE and _uid for private problems.
    echo Run 'config_cookie.bat' to set up your cookie and uid.
    exit /b 1
)