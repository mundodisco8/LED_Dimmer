@echo off
SETLOCAL

:: --- CONFIGURATION ---
SET "PROJECT_FILE=project.cppcheck"
SET "BUILD_DIR=cppcheck_build"
SET "OUTPUT_XML=report.xml"
SET "REPORT_DIR=cppCheck_html_report"
:: Update this path to where your Cppcheck is installed
SET "CPPCHECK_PATH=C:\Program Files\Cppcheck"

:: 1. Ensure build directory exists
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

echo [1/2] Running Cppcheck analysis...
"%CPPCHECK_PATH%\cppcheck.exe" --enable=warning,performance,portability,style --inline-suppr --project=%PROJECT_FILE% --std=c11 --xml --xml-version=2 2> %OUTPUT_XML%

if %ERRORLEVEL% NEQ 0 (
    echo Cppcheck encountered an error.
    exit /b %ERRORLEVEL%
)

echo [2/2] Generating HTML report...
:: Use the python script bundled with Cppcheck
python "%CPPCHECK_PATH%\htmlreport\cppcheck-htmlreport.py" ^
    --file=%OUTPUT_XML% ^
    --report-dir=%REPORT_DIR% ^
    --source-dir=. ^
    --title="BGM220 Project Analysis"

echo.
echo Analysis Complete!
echo Open %REPORT_DIR%\index.html to view results.
