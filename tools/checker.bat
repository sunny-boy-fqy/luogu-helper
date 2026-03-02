@echo off
REM checker.bat - Compare main.cpp with std.cpp using generated test data (Windows version)
REM Usage: checker.bat [num_tests] [timeout_seconds]

setlocal EnableDelayedExpansion

REM Get tools directory
set "TOOLS_DIR=%~dp0"
set "TOOLS_DIR=%TOOLS_DIR:~0,-1%"

REM Parse arguments
if "%~1"=="" (set NUM_TESTS=100) else (set NUM_TESTS=%~1)
if "%~2"=="" (set TIMEOUT_SEC=2) else (set TIMEOUT_SEC=%~2)

REM Find problem directory
cd /d "%~dp0\..\.."
set "PROB_DIR=%CD%"
for %%F in ("%CD%") do set "PROB_ID=%%~nxF"

echo ========================================
echo   Checker: %PROB_ID%
echo   Tests: %NUM_TESTS% ^| Timeout: %TIMEOUT_SEC%s
echo ========================================
echo.

REM Check required files
if not exist "%PROB_DIR%\gen.cpp" (
    echo Error: gen.cpp not found!
    echo Please create gen.cpp or copy from std\gen.cpp
    exit /b 1
)

if not exist "%PROB_DIR%\main.cpp" (
    echo Error: main.cpp not found!
    exit /b 1
)

if not exist "%PROB_DIR%\std.cpp" (
    echo Warning: std.cpp not found!
    echo Attempting to fetch from Luogu...
    if exist "%TOOLS_DIR%\get_solve.bat" (
        call "%TOOLS_DIR%\get_solve.bat"
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
set "TEMP_DIR=%TEMP%\checker_%RANDOM%"
mkdir "%TEMP_DIR%"

REM Compile programs
echo Compiling...
echo   - gen.cpp
g++ -std=c++17 -O2 "%PROB_DIR%\gen.cpp" -o "%TEMP_DIR%\gen.exe" 2>&1
if %ERRORLEVEL% neq 0 (
    echo Error: Failed to compile gen.cpp
    rmdir /S /Q "%TEMP_DIR%"
    exit /b 1
)

echo   - main.cpp
g++ -std=c++17 -O2 "%PROB_DIR%\main.cpp" -o "%TEMP_DIR%\main.exe" 2>&1
if %ERRORLEVEL% neq 0 (
    echo Error: Failed to compile main.cpp
    rmdir /S /Q "%TEMP_DIR%"
    exit /b 1
)

echo   - std.cpp
g++ -std=c++17 -O2 "%PROB_DIR%\std.cpp" -o "%TEMP_DIR%\std.exe" 2>&1
if %ERRORLEVEL% neq 0 (
    echo Error: Failed to compile std.cpp
    rmdir /S /Q "%TEMP_DIR%"
    exit /b 1
)

echo.
echo Running tests...
echo.

set FAILED=0
for /L %%i in (1,1,%NUM_TESTS%) do (
    REM Generate test data
    "%TEMP_DIR%\gen.exe" > "%TEMP_DIR%\input.txt"
    
    REM Run main.cpp with timeout
    timeout /T %TIMEOUT_SEC% /NOBREAK >nul 2>&1
    "%TEMP_DIR%\main.exe" < "%TEMP_DIR%\input.txt" > "%TEMP_DIR%\output_main.txt" 2>nul
    if %ERRORLEVEL% neq 0 (
        echo Test %%i: main.cpp TLE or crashed
        copy "%TEMP_DIR%\input.txt" "%PROB_DIR%\fail.in" >nul
        set FAILED=1
        goto :cleanup
    )
    
    REM Run std.cpp with timeout
    "%TEMP_DIR%\std.exe" < "%TEMP_DIR%\input.txt" > "%TEMP_DIR%\output_std.txt" 2>nul
    if %ERRORLEVEL% neq 0 (
        echo Test %%i: std.cpp TLE or crashed
        set FAILED=1
        goto :cleanup
    )
    
    REM Compare outputs (simple comparison)
    fc /W "%TEMP_DIR%\output_main.txt" "%TEMP_DIR%\output_std.txt" >nul 2>&1
    if %ERRORLEVEL% neq 0 (
        echo Test %%i: WRONG ANSWER
        echo.
        echo Input:
        type "%TEMP_DIR%\input.txt"
        echo.
        echo Your output ^(main.cpp^):
        type "%TEMP_DIR%\output_main.txt"
        echo.
        echo Expected ^(std.cpp^):
        type "%TEMP_DIR%\output_std.txt"
        echo.
        copy "%TEMP_DIR%\input.txt" "%PROB_DIR%\fail.in" >nul
        copy "%TEMP_DIR%\output_std.txt" "%PROB_DIR%\fail.out" >nul
        echo Failing test saved to: fail.in, fail.out
        set FAILED=1
        goto :cleanup
    )
    
    REM Progress report every 10 tests
    set /A MOD=%%i %% 10
    if !MOD!==0 (
        echo   Passed %%i/%NUM_TESTS% tests...
    )
)

:cleanup
echo.
echo ========================================
if %FAILED%==0 (
    echo   All %NUM_TESTS% tests passed!
    echo ========================================
    rmdir /S /Q "%TEMP_DIR%"
    exit /b 0
) else (
    echo   Stopped at test
    echo ========================================
    rmdir /S /Q "%TEMP_DIR%"
    exit /b 1
)
