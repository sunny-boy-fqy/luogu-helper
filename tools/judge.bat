@echo off
REM Global judge wrapper for Windows
REM Auto-recompiles judge.cpp if it's newer than the binary

set "TOOLS_DIR=%~dp0"
set "TOOLS_DIR=%TOOLS_DIR:~0,-1%"
set "JUDGE_CPP=%TOOLS_DIR%\judge.cpp"
set "JUDGE_BIN=%TOOLS_DIR%\judge.exe"

REM Check if recompilation is needed
set COMPILE=0
if not exist "%JUDGE_BIN%" set COMPILE=1
if exist "%JUDGE_CPP%" (
    for %%F in ("%JUDGE_CPP%") do set CPP_TIME=%%~tF
    for %%F in ("%JUDGE_BIN%") do set BIN_TIME=%%~tF
    if "%CPP_TIME%" gtr "%BIN_TIME%" set COMPILE=1
)

if %COMPILE%==1 (
    echo Recompiling judge...
    g++ "%JUDGE_CPP%" -O2 -o "%JUDGE_BIN%"
    if %ERRORLEVEL% neq 0 (
        echo Error: Failed to compile judge.cpp
        exit /b 1
    )
)

REM Execute the judge binary in the current directory
"%JUDGE_BIN%" %*
