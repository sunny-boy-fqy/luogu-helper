@echo off
setlocal enabledelayedexpansion
REM OI Problem setup script for Windows
REM Usage: new.bat <PROBLEM_ID_OR_FOLDER_NAME>

if "%~1"=="" (
    echo Usage: new.bat ^<PROBLEM_ID_OR_FOLDER_NAME^>
    exit /b 1
)

set TARGET=%~1
set PROB_ID=%TARGET%

REM Convert to uppercase
for %%i in (A B C D E F G H I J K L M N O P Q R S T U V W X Y Z) do (
    set PROB_ID=!PROB_ID:%%i=%%i!
)

REM Get script directory
set SCRIPT_DIR=%~dp0
set PROG_DIR=%SCRIPT_DIR%
set STD_DIR=%PROG_DIR%std
set FETCH_SCRIPT=%PROG_DIR%tools\fetch_problem.py

REM Check if it's a problem ID (P/B/U/T followed by digits, or CF format, or contains underscore)
echo %PROB_ID% | findstr /R "^P[0-9]*$ ^B[0-9]*$ ^U[0-9]*$ ^T[0-9]*$" >nul 2>&1
if %errorlevel%==0 goto :is_problem_id

echo %PROB_ID% | findstr /R "^[0-9][0-9]*[A-Z]" >nul 2>&1
if %errorlevel%==0 goto :is_problem_id

echo %TARGET% | findstr /R "_" >nul 2>&1
if %errorlevel%==0 goto :is_problem_id

REM Not a problem ID, just create a directory
echo Creating directory: %TARGET%
xcopy /E /I /Q "%STD_DIR%" "%PROG_DIR%%TARGET%\" >nul 2>&1
echo Successfully set up %TARGET%
echo Please run: cd %PROG_DIR%%TARGET%
exit /b 0

:is_problem_id
echo Detected problem ID: %TARGET%

if exist "%PROG_DIR%%TARGET%" (
    echo Error: Directory %TARGET% already exists.
    exit /b 1
)

xcopy /E /I /Q "%STD_DIR%" "%PROG_DIR%%TARGET%\" >nul 2>&1
python "%FETCH_SCRIPT%" samples %TARGET% --target_dir "%PROG_DIR%%TARGET%" %2 %3 %4 %5

if %errorlevel%==0 (
    echo Successfully set up %PROB_ID%
    echo Please run: cd %PROG_DIR%%TARGET%
) else (
    echo Failed to fetch samples, but directory created.
)