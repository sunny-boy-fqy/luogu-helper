@echo off
REM Configure Luogu cookie and uid for Windows
REM Usage: config_cookie.bat

setlocal enabledelayedexpansion

REM Get script directory
set SCRIPT_DIR=%~dp0

REM Create .luogu_cookie file in user's home directory
set COOKIE_FILE=%USERPROFILE%\.luogu_cookie

echo ========================================
echo   Luogu Helper - Cookie Configuration
echo ========================================
echo.
echo This will configure your Luogu cookie for accessing private problems.
echo.
echo To get your cookie:
echo 1. Log in to https://www.luogu.com.cn
echo 2. Open Developer Tools (F12)
echo 3. Go to Application ^> Cookies
echo 4. Find _uid and __client_id values
echo.
pause

echo Please enter your __client_id:
set /p CLIENT_ID=

echo Please enter your _uid:
set /p UID=

echo.
echo Saving cookie to: %COOKIE_FILE%
echo __client_id=!CLIENT_ID!; _uid=!UID! > "%COOKIE_FILE%"

echo.
echo Cookie configured successfully!
echo You can now access private problems.
echo.
echo Note: The cookie file is stored at:
echo   %COOKIE_FILE%
echo.
echo If you need to update your cookie later, run this script again.

echo.
pause