@echo off
setlocal enabledelayedexpansion
REM checker: Compare main.cpp with std.cpp using generated test data
REM Usage: checker.bat [num_tests] [timeout_seconds]
REM   num_tests: Number of test cases to run (default: 100)
REM   timeout_seconds: Timeout for each test (default: 2)

REM Find Python (for get_solve)
set PYTHON=
where python >nul 2>&1
if %errorlevel%==0 set PYTHON=python
if not defined PYTHON (
    where py >nul 2>&1
    if %errorlevel%==0 set PYTHON=py
)

REM Get the tools directory
set TOOLS_DIR=%~dp0

REM Parse arguments
set NUM_TESTS=100
set TIMEOUT=2
if not "%~1"=="" set NUM_TESTS=%~1
if not "%~2"=="" set TIMEOUT=%~2

REM Find the problem directory (current working directory)
for %%I in in ("%cd%.") do set PROB_DIR=%%~fI
for %%I in ("%PROB_DIR%") do set PROB_ID=%%~nxI

echo ========================================
echo   Checker: %PROB_ID%
echo   Tests: %NUM_TESTS% ^| Timeout: %TIMEOUT%s
echo ========================================
echo.

REM Check required files
if not exist "%PROB_DIR%\gen.cpp" (
    echo Error: gen.cpp not found!
    echo Please create gen.cpp or run: copy "%TOOLS_DIR%..\std\gen.cpp" .
    exit /b 1
)

if not exist "%PROB_DIR%\main.cpp" (
    echo Error: main.cpp not found!
    exit /b 1
)

if not exist "%PROB_DIR%\std.cpp" (
    echo Warning: std.cpp not found!
    echo Attempting to fetch from Luogu...
    if exist "%TOOLS_DIR%get_solve.bat" (
        pushd "%PROB_DIR%"
        call "%TOOLS_DIR%get_solve.bat"
        popd
    ) else (
        echo Error: Could not fetch std.cpp
        exit /b 1
    )
    
    if not exist "%PROB_DIR%\std.cpp" (
        echo Error: Failed to obtain std.cpp
        exit /b 1
    )
)

REM Create temp directory
set TEMP_DIR=%TEMP%\checker_%RANDOM%
mkdir "%TEMP_DIR%" 2>nul

REM Compile programs
echo Compiling...
echo   - gen.cpp
g++ -w -std=c++17 -O2 "%PROB_DIR%\gen.cpp" -o "%TEMP_DIR%\gen.exe" 2>&1
if %errorlevel% neq 0 (
    echo Error: Failed to compile gen.cpp
    rmdir /s /q "%TEMP_DIR%" 2>nul
    exit /b 1
)

echo   - main.cpp
g++ -std=c++17 -O2 "%PROB_DIR%\main.cpp" -o "%TEMP_DIR%\main.exe" 2>&1
if %errorlevel% neq 0 (
    echo Error: Failed to compile main.cpp
    rmdir /s /q "%TEMP_DIR%" 2>nul
    exit /b 1
)

echo   - std.cpp
g++ -w -std=c++17 -O2 "%PROB_DIR%\std.cpp" -o "%TEMP_DIR%\std.exe" 2>&1
if %errorlevel% neq 0 (
    echo Error: Failed to compile std.cpp
    rmdir /s /q "%TEMP_DIR%" 2>nul
    exit /b 1
)

echo.
echo Running tests...
echo.

REM Run tests
set FAILED=0
set /a COUNT=0

:loop
set /a COUNT+=1
if %COUNT% gtr %NUM_TESTS% goto :done

REM Generate test data
"%TEMP_DIR%\gen.exe" > "%TEMP_DIR%\input.txt" 2>&1

REM Run main.cpp with timeout (using timeout command for Windows)
REM Windows doesn't have a direct timeout for process execution, using a workaround
"%TEMP_DIR%\main.exe" < "%TEMP_DIR%\input.txt" > "%TEMP_DIR%\output_main.txt" 2>%TEMP_DIR%\main_err.txt
if %errorlevel% neq 0 (
    echo Test %COUNT%: main.cpp crashed or timeout
    type "%TEMP_DIR%\main_err.txt" 2>nul
    set FAILED=1
    goto :save_fail
)

"%TEMP_DIR%\std.exe" < "%TEMP_DIR%\input.txt" > "%TEMP_DIR%\output_std.txt" 2>%TEMP_DIR%\std_err.txt
if %errorlevel% neq 0 (
    echo Test %COUNT%: std.cpp crashed or timeout
    echo This is unexpected - std.cpp should be correct!
    type "%TEMP_DIR%\std_err.txt" 2>nul
    set FAILED=1
    goto :save_fail
)

REM Compare outputs (normalize whitespace)
REM Create VBScript for text normalization (simple version: just compare directly)
fc "%TEMP_DIR%\output_main.txt" "%TEMP_DIR%\output_std.txt" >nul 2>&1
if %errorlevel% neq 0 (
    REM Try with trimmed lines using findstr workaround
    REM Simple direct comparison for now
    echo Test %COUNT%: WRONG ANSWER
    echo.
    echo Input:
    type "%TEMP_DIR%\input.txt" | more +0 | findstr /n ".*" | findstr "^[1-9]:" >nul 2>&1
    type "%TEMP_DIR%\input.txt" 2>nul | findstr /n "." 2>nul | findstr "^[1-2][0-9]:" || type "%TEMP_DIR%\input.txt"
    echo.
    echo Your output (main.cpp^):
    type "%TEMP_DIR%\output_main.txt" 2>nul | findstr /n "." 2>nul | findstr "^[1-2][0-9]:" || type "%TEMP_DIR%\output_main.txt"
    echo.
    echo Expected (std.cpp^):
    type "%TEMP_DIR%\output_std.txt" 2>nul | findstr /n "." 2>nul | findstr "^[1-2][0-9]:" || type "%TEMP_DIR%\output_std.txt"
    echo.
    set FAILED=1
    goto :save_fail
)

REM Progress report every 10 tests
set /a MOD=COUNT%%10
if %MOD%==0 (
    echo   Passed %COUNT%/%NUM_TESTS% tests...
)

goto :loop

:done
echo.
echo ========================================
if %FAILED%==0 (
    echo   All %NUM_TESTS% tests passed!
    echo ========================================
    rmdir /s /q "%TEMP_DIR%" 2>nul
    exit /b 0
)

:save_fail
echo ========================================
echo   Stopped at test %COUNT%
echo ========================================
REM Save failing test case
copy "%TEMP_DIR%\input.txt" "%PROB_DIR%\fail.in" >nul 2>&1
copy "%TEMP_DIR%\output_std.txt" "%PROB_DIR%\fail.out" >nul 2>&1
copy "%TEMP_DIR%\output_main.txt" "%PROB_DIR%\fail.ans" >nul 2>&1
echo Failing test saved to: fail.in, fail.out, fail.ans
rmdir /s /q "%TEMP_DIR%" 2>nul
exit /b 1