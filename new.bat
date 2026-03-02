@echo off
REM OI Problem setup script for Windows
REM Usage: new.bat <PROBLEM_ID_OR_FOLDER_NAME>

if "%~1"=="" (
    echo Usage: new.bat ^<PROBLEM_ID_OR_FOLDER_NAME^>
    exit /b 1
)

set "TARGET=%~1"
set "PROB_ID=%~1"

REM Get script directory
set "SCRIPT_DIR=%~dp0"
set "PROG_DIR=%SCRIPT_DIR:~0,-1%"
set "STD_DIR=%PROG_DIR%\std"
set "FETCH_SCRIPT=%PROG_DIR%\tools\fetch_problem.py"

REM Check if it's a problem ID (simple heuristic)
echo %PROB_ID% | findstr /R "^[PBUT][0-9][0-9]*$" >nul
set IS_PROB_ID=%ERRORLEVEL%

echo %PROB_ID% | findstr /R "^[0-9][0-9]*[A-Z]" >nul
if %ERRORLEVEL%==0 set IS_PROB_ID=0

echo %TARGET% | findstr "_" >nul
if %ERRORLEVEL%==0 set IS_PROB_ID=0

if %IS_PROB_ID%==0 (
    echo Detected problem ID: %TARGET%
    
    if exist "%PROG_DIR%\%TARGET%" (
        echo Error: Directory %TARGET% already exists.
        exit /b 1
    )
    
    xcopy /E /I "%STD_DIR%" "%PROG_DIR%\%TARGET%" >nul
    python "%FETCH_SCRIPT%" samples "%TARGET%" --target_dir "%PROG_DIR%\%TARGET%"
    
    if %ERRORLEVEL%==0 (
        echo Successfully set up %TARGET%
        echo Please run: cd %PROG_DIR%\%TARGET%
    ) else (
        echo Failed to fetch samples, but directory created.
    )
) else (
    echo Creating directory: %TARGET%
    xcopy /E /I "%STD_DIR%" "%PROG_DIR%\%TARGET%" >nul
    echo Successfully set up %TARGET%
    echo Please run: cd %PROG_DIR%\%TARGET%
)
