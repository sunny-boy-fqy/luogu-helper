@echo off
REM clear.bat: Sync core OI files and main.cpp's to Google Drive, then clean up (Windows version)
REM Usage: clear.bat

set "REMOTE=gdrive:OI/program"

REM Get script directory
set "SCRIPT_DIR=%~dp0"
set "LOCAL_DIR=%SCRIPT_DIR:~0,-1%"

echo Starting sync to Google Drive (%REMOTE%)...

REM Check if rclone is installed
where rclone >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Error: rclone is not installed!
    echo Please install rclone first: https://rclone.org/install/
    exit /b 1
)

REM Sync to Google Drive
rclone sync "%LOCAL_DIR%" "%REMOTE%" -L ^
    --filter "+ /std/**" ^
    --filter "+ /tools/**" ^
    --filter "+ /include/**" ^
    --filter "+ /.vscode/**" ^
    --filter "+ /new.bat" ^
    --filter "+ /clear.bat" ^
    --filter "+ /sync.bat" ^
    --filter "+ /README.md" ^
    --filter "+ **/main.cpp" ^
    --filter "- *" ^
    --progress

if %ERRORLEVEL% equ 0 (
    echo Sync completed successfully (Only main.cpp and tools uploaded).
    echo Cleaning up local directory...
    
    REM List of items to keep
    set "KEEP_LIST=std tools new.bat clear.bat sync.bat include .vscode README.md"
    
    for /D %%D in ("%LOCAL_DIR%\*") do (
        set "DIR_NAME=%%~nxD"
        set "KEEP=0"
        for %%K in (%KEEP_LIST%) do (
            if "!DIR_NAME!"=="%%K" set "KEEP=1"
        )
        if !KEEP!==0 (
            echo Deleting: %%~nxD
            rmdir /S /Q "%%D"
        )
    )
    
    for %%F in ("%LOCAL_DIR%\*") do (
        set "FILE_NAME=%%~nxF"
        set "KEEP=0"
        for %%K in (%KEEP_LIST%) do (
            if "!FILE_NAME!"=="%%K" set "KEEP=1"
        )
        if !KEEP!==0 (
            echo Keeping: %%~nxF
        ) else (
            echo Deleting: %%~nxF
            del /F /Q "%%F"
        )
    )
    
    echo Cleanup finished.
) else (
    echo Sync failed! Cleanup aborted.
    exit /b 1
)
