@echo off
REM Get problem statement for Windows

set "TOOLS_DIR=%~dp0"
set "TOOLS_DIR=%TOOLS_DIR:~0,-1%"

for %%F in ("%CD%") do set "PROB_ID=%%~nxF"
python "%TOOLS_DIR%\fetch_problem.py" problem "%PROB_ID%" --target_dir .
