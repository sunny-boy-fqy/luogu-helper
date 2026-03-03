@echo off
setlocal enabledelayedexpansion

REM judge: Local judge for competitive programming
REM Usage: judge.bat [OPTIONS]
REM   -t SECONDS   Override time limit
REM   -c COMPILER  Compiler to use (default: g++)
REM   -h           Show this help message

REM Default values
set TIME_LIMIT_SEC=
set COMPILER=g++
set TIME_LIMIT_MS=1000

REM Parse arguments
:parse_args
if "%~1"=="" goto :done_args
if /i "%~1"=="-h" goto :show_help
if /i "%~1"=="--help" goto :show_help
if /i "%~1"=="-t" (
    set TIME_LIMIT_SEC=%~2
    shift
    shift
    goto :parse_args
)
if /i "%~1"=="-c" (
    set COMPILER=%~2
    shift
    shift
    goto :parse_args
)
echo Unknown option: %~1
exit /b 1

:show_help
echo Usage: judge.bat [OPTIONS]
echo Options:
echo   -t SECONDS   Override time limit
echo   -c COMPILER  Compiler to use (default: g++)
echo   -h            Show this help message
exit /b 0

:done_args

REM Check if main.cpp exists
if not exist "main.cpp" (
    echo Error: main.cpp not found!
    exit /b 1
)

REM Check if info file exists
set INFO_FILE=data\info
if not exist "!INFO_FILE!" (
    set INFO_FILE=info
)
if not exist "!INFO_FILE!" (
    echo Error: info file not found!
    echo Please create data\info or info file with format: num_tests time_limit_ms
    exit /b 1
)

REM Read info file
set /p INFO_LINE=<"!INFO_FILE!"
for /f "tokens=1,2" %%a in ("!INFO_LINE!") do (
    set NUM_TESTS=%%a
    set TIME_LIMIT_MS=%%b
)

REM Calculate timeout in seconds (ceiling)
set /a TIME_LIMIT_CEIL=(TIME_LIMIT_MS + 999) / 1000
if defined TIME_LIMIT_SEC set TIME_LIMIT_CEIL=%TIME_LIMIT_SEC%

echo ========================================
echo   Judge: %CD%
echo   Tests: %NUM_TESTS% ^| Time limit: %TIME_LIMIT_MS%ms
echo ========================================
echo.

REM Compile main.cpp
echo Compiling main.cpp...
%COMPILER% -std=c++17 -O2 main.cpp -o judge_temp.exe 2>&1
if %errorlevel% neq 0 (
    echo Compilation Error!
    exit /b 1
)
echo Compilation successful
echo.

REM Check for special judge
set HAS_SPJ=0
if exist "data\spj.exe" set HAS_SPJ=1
if exist "data\spj" set HAS_SPJ=1

REM Run tests
set TOTAL_AC=1
set FAILED_TEST=0
set FIRST_FAILED=

for /l %%i in (1,1,%NUM_TESTS%) do (
    if !TOTAL_AC!==1 (
        set INPUT_FILE=data\data%%i.in
        set OUTPUT_FILE=data\data%%i.out
        
        if not exist "!INPUT_FILE!" (
            echo Test %%i: Missing input file
            set TOTAL_AC=0
            set FAILED_TEST=%%i
            set FIRST_FAILED=Missing input file
            goto :end_tests
        )
        
        if not exist "!OUTPUT_FILE!" (
            echo Test %%i: Missing output file
            set TOTAL_AC=0
            set FAILED_TEST=%%i
            set FIRST_FAILED=Missing output file
            goto :end_tests
        )
        
        REM Run the program
        judge_temp.exe < "!INPUT_FILE!" > judge_user_output_%%i.txt 2>nul
        set EXIT_CODE=!errorlevel!
        
        REM Check exit code
        if !EXIT_CODE! neq 0 (
            echo Test %%i: RE ^(exit code: !EXIT_CODE!^)
            set TOTAL_AC=0
            set FAILED_TEST=%%i
            set FIRST_FAILED=Runtime Error
            del judge_user_output_%%i.txt 2>nul
            goto :end_tests
        )
        
        REM Compare outputs
        if !HAS_SPJ!==1 (
            data\spj "!INPUT_FILE!" judge_user_output_%%i.txt "!OUTPUT_FILE!" >nul 2>&1
            if !errorlevel! equ 0 (
                echo Test %%i: AC
            ) else (
                echo Test %%i: WA
                set TOTAL_AC=0
                set FAILED_TEST=%%i
                set FIRST_FAILED=Wrong Answer ^(SPJ^)
                del judge_user_output_%%i.txt 2>nul
                goto :end_tests
            )
        ) else (
            REM Compare files
            fc /L "!OUTPUT_FILE!" judge_user_output_%%i.txt >nul 2>&1
            if !errorlevel! equ 0 (
                echo Test %%i: AC
            ) else (
                echo Test %%i: WA
                set TOTAL_AC=0
                set FAILED_TEST=%%i
                set FIRST_FAILED=Wrong Answer
                
                echo.
                echo Input:
                type "!INPUT_FILE!" 2>nul | findstr /n "." | findstr "^[1-2][0-9]:" >nul 2>&1 || type "!INPUT_FILE!"
                echo.
                echo Your output:
                type judge_user_output_%%i.txt 2>nul
                echo.
                echo Expected:
                type "!OUTPUT_FILE!" 2>nul
                
                REM Save failed test
                copy "!INPUT_FILE!" fail.in >nul 2>&1
                copy judge_user_output_%%i.txt fail.out >nul 2>&1
                copy "!OUTPUT_FILE!" fail.ans >nul 2>&1
                echo Failed test saved to fail.in, fail.out, fail.ans
                
                del judge_user_output_%%i.txt 2>nul
                goto :end_tests
            )
        )
        
        del judge_user_output_%%i.txt 2>nul
    )
)

:end_tests

REM Cleanup
del judge_temp.exe 2>nul

echo.
echo ========================================
if %TOTAL_AC%==1 (
    echo   All tests passed!
    echo ========================================
    exit /b 0
) else (
    echo   Failed at test %FAILED_TEST%
    echo   Reason: %FIRST_FAILED%
    echo ========================================
    exit /b 1
)