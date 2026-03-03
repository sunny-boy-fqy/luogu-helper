@echo off
setlocal enabledelayedexpansion
REM clear: Sync core OI files to Google Drive, then clean up.
REM Usage: clear.bat

set REMOTE=gdrive:OI/program

REM Get script directory
set SCRIPT_DIR=%~dp0
set LOCAL_DIR=%SCRIPT_DIR%

REM Files to KEEP locally
set KEEP_LIST=std tools new new.bat clear clear.bat include .vscode sync sync.bat .gitignore README.md README_en.md README_windows.md

echo Starting sync to Google Drive (%REMOTE%)...
echo.

REM Check if rclone is installed
where rclone >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: rclone is not installed!
    echo Please install rclone from https://rclone.org/install/
    exit /b 1
)

REM Check if remote is configured
rclone listremotes 2>nul | findstr "gdrive:" >nul
if %errorlevel% neq 0 (
    echo Error: Google Drive remote 'gdrive' not configured!
    echo Please run: rclone config
    exit /b 1
)

echo WARNING: This will sync files and then clean up local directory.
echo Press Ctrl+C to cancel, or
pause

REM Sync to remote
rclone sync "%LOCAL_DIR%" %REMOTE% -L ^
    --filter "+ /std/**" ^
    --filter "+ /tools/**" ^
    --filter "+ /include/**" ^
    --filter "+ /.vscode/**" ^
    --filter "+ /new" ^
    --filter "+ /new.bat" ^
    --filter "+ /clear" ^
    --filter "+ /clear.bat" ^
    --filter "+ /sync" ^
    --filter "+ /sync.bat" ^
    --filter "+ **/main.cpp" ^
    --filter "+ /README.md" ^
    --filter "+ /README_en.md" ^
    --filter "+ /README_windows.md" ^
    --filter "+ /.gitignore" ^
    --filter "- *" ^
    --progress

if %errorlevel% neq 0 (
    echo.
    echo Sync failed! Cleanup aborted.
    exit /b 1
)

echo.
echo Sync completed successfully (Only main.cpp and tools uploaded).
echo Cleaning up local directory...
echo.

REM Clean up local directory
for /d %%d in ("%LOCAL_DIR%*") do (
    set basename=%%~nxd
    set keep=0
    for %%k in (%KEEP_LIST%) do (
        if "!basename!"=="%%k" set keep=1
    )
    if !keep!==0 (
        echo Deleting: !basename!
        rmdir /s /q "%%d" 2>nul
    )
)

for %%f in ("%LOCAL_DIR%*") do (
    set basename=%%~nxf
    set keep=0
    for %%k in (%KEEP_LIST%) do (
        if "!basename!"=="%%k" set keep=1
    )
    if !keep!==0 (
        echo Deleting: !basename!
        del /q "%%f" 2>nul
    )
)

echo.
echo Cleanup finished.