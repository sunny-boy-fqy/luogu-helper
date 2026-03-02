@echo off
REM sync.bat: Sync core OI files from Google Drive to local (Windows version)
REM Usage: sync.bat

set "REMOTE=gdrive:OI/program"

REM Get script directory
set "SCRIPT_DIR=%~dp0"
set "LOCAL_DIR=%SCRIPT_DIR:~0,-1%"

echo Starting sync FROM Google Drive (%REMOTE%)...
echo Local directory: %LOCAL_DIR%
echo.

REM Check if rclone is installed
where rclone >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Error: rclone is not installed!
    echo Please install rclone first: https://rclone.org/install/
    exit /b 1
)

REM Check if remote is configured
rclone listremotes | findstr "^gdrive:" >nul
if %ERRORLEVEL% neq 0 (
    echo Error: Google Drive remote 'gdrive' not configured!
    echo Please run: rclone config
    exit /b 1
)

echo Syncing core files from remote to local...
rclone sync "%REMOTE%" "%LOCAL_DIR%" -L ^
    --filter "+ /std/**" ^
    --filter "+ /tools/**" ^
    --filter "+ /include/**" ^
    --filter "+ /.vscode/**" ^
    --filter "+ /new.bat" ^
    --filter "+ /clear.bat" ^
    --filter "+ /sync.bat" ^
    --filter "+ /README.md" ^
    --filter "+ /new" ^
    --filter "+ /clear" ^
    --filter "+ /sync" ^
    --filter "- *" ^
    --progress ^
    --delete-after

if %ERRORLEVEL% equ 0 (
    echo.
    echo Sync completed successfully!
    echo.
    echo Synced items:
    echo   - std/
    echo   - tools/
    echo   - include/
    echo   - .vscode/
    echo   - new/new.bat
    echo   - clear/clear.bat
    echo   - sync/sync.bat
    echo   - README.md
    echo.
    echo Problem directories are preserved.
) else (
    echo.
    echo Sync failed!
    exit /b 1
)
