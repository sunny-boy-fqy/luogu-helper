@echo off
setlocal enabledelayedexpansion
REM sync: Sync core OI files from Google Drive to local.
REM Usage: sync.bat

set REMOTE=gdrive:OI/program

REM Get script directory
set SCRIPT_DIR=%~dp0
set LOCAL_DIR=%SCRIPT_DIR%

echo Starting sync FROM Google Drive (%REMOTE%)...
echo Local directory: %LOCAL_DIR%
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

echo Syncing core files from remote to local...

rclone sync %REMOTE% "%LOCAL_DIR%" -L ^
    --filter "+ /include/**" ^
    --filter "+ /tools/**" ^
    --filter "+ /std/**" ^
    --filter "+ /.vscode/**" ^
    --filter "+ /clear" ^
    --filter "+ /clear.bat" ^
    --filter "+ /new" ^
    --filter "+ /new.bat" ^
    --filter "+ /sync" ^
    --filter "+ /sync.bat" ^
    --filter "- *" ^
    --progress ^
    --delete-after

if %errorlevel%==0 (
    echo.
    echo Sync completed successfully!
    echo.
    echo Synced items:
    echo   ^+ include
    echo   ^+ tools
    echo   ^+ std
    echo   ^+ .vscode
    echo   ^+ clear, new, sync scripts
    echo.
    echo Problem directories are preserved.
) else (
    echo.
    echo Sync failed!
    exit /b 1
)