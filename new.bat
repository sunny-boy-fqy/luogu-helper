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
set PROB_ID_UPPER=%PROB_ID%
for %%i in (A B C D E F G H I J K L M N O P Q R S T U V W X Y Z) do (
    set PROB_ID_UPPER=!PROB_ID_UPPER:%%i=%%i!
)

echo !PROB_ID_UPPER! | findstr /R "^P[0-9]*$ ^B[0-9]*$ ^U[0-9]*$ ^T[0-9]*$" >nul 2>&1
if %errorlevel%==0 goto :is_problem_id

echo !PROB_ID_UPPER! | findstr /R "^[0-9][0-9]*[A-Z]" >nul 2>&1
if %errorlevel%==0 goto :is_problem_id

echo %TARGET% | findstr /R "_" >nul 2>&1
if %errorlevel%==0 goto :is_problem_id

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
echo Please run: cd %SCRIPT_DIR%%TARGET%
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
"%FETCH_EXE%" samples %TARGET% --target_dir "%SCRIPT_DIR%%TARGET%"

if %errorlevel%==0 (
    echo Successfully set up %PROB_ID%
    echo Please run: cd %SCRIPT_DIR%%TARGET%
) else (
    echo Failed to fetch samples, but directory created.
    echo You may need to configure LUOGU_COOKIE and _uid for private problems.
    echo Run 'config_cookie.bat' to set up your cookie and uid.
)

set TARGET=%~1
set PROB_ID=%TARGET%

REM Get script directory (use %~dp0 directly)
set SCRIPT_DIR=%~dp0
set STD_DIR=%SCRIPT_DIR%std

REM Check if it's a problem ID
set PROB_ID_UPPER=%PROB_ID%
for %%i in (A B C D E F G H I J K L M N O P Q R S T U V W X Y Z) do (
    set PROB_ID_UPPER=!PROB_ID_UPPER:%%i=%%i!
)

echo !PROB_ID_UPPER! | findstr /R "^P[0-9]*$ ^B[0-9]*$ ^U[0-9]*$ ^T[0-9]*$" >nul 2>&1
if %errorlevel%==0 goto :is_problem_id

echo !PROB_ID_UPPER! | findstr /R "^[0-9][0-9]*[A-Z]" >nul 2>&1
if %errorlevel%==0 goto :is_problem_id

echo %TARGET% | findstr /R "_" >nul 2>&1
if %errorlevel%==0 goto :is_problem_id

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
echo Please run: cd %SCRIPT_DIR%%TARGET%
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

REM Check if fetch_problem.exe exists
set FETCH_EXE=%SCRIPT_DIR%tools\fetch_problem.exe
if not exist "%FETCH_EXE%" (
    echo Error: fetch_problem.exe not found at %FETCH_EXE%
    echo Please ensure you downloaded the complete Windows release package.
    exit /b 1
)

echo Fetching samples from Luogu...
"%FETCH_EXE%" samples %TARGET% --target_dir "%SCRIPT_DIR%%TARGET%"

if %errorlevel%==0 (
    echo Successfully set up %PROB_ID%
    echo Please run: cd %SCRIPT_DIR%%TARGET%
) else (
    echo Failed to fetch samples, but directory created.
)