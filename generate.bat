@echo off
setlocal EnableDelayedExpansion
cd /d "%~dp0"

set "VSYEAR=%~1"
set "DOBUILD=%~2"
set "AUTO="

where cmake >nul 2>&1
if errorlevel 1 (
    echo Install CMake.
    set "RC=1" & goto :end
)

if not exist "thirdparty\imgui\.git" (
    echo Submodules missing, Running git submodule update recursively...
    git submodule update --init --recursive
    if errorlevel 1 (
        echo git submodule update failed.
        set "RC=1" & goto :end
    )
)

if not "%VSYEAR%"=="" goto :have_year
set "AUTO=1"

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" set "VSWHERE=%ProgramFiles%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" goto :probe_dirs

for /f "usebackq tokens=1 delims=." %%v in (`"%VSWHERE%" -latest -products * -property installationVersion 2^>nul`) do set "VSYEAR=%%v"
if not "%VSYEAR%"=="" goto :have_year

:probe_dirs
if exist "%ProgramFiles%\Microsoft Visual Studio\18\" set "VSYEAR=2026" & goto :have_year
if exist "%ProgramFiles%\Microsoft Visual Studio\2022\" set "VSYEAR=2022" & goto :have_year

echo Could not detect a Visual Studio install.
echo Try to pass the version: generate.bat 2022
set "RC=1" & goto :end

:have_year
if "%VSYEAR%"=="17" set "VSYEAR=2022"
if "%VSYEAR%"=="18" set "VSYEAR=2026"
if defined AUTO echo Detected Visual Studio %VSYEAR%.

set "GENERATOR="
if "%VSYEAR%"=="2022" set "GENERATOR=Visual Studio 17 2022"
if "%VSYEAR%"=="2026" set "GENERATOR=Visual Studio 18 2026"

if not defined GENERATOR (
    echo Unsupported Visual Studio version "%VSYEAR%".
    echo Supported: 2022, 2026
    set "RC=1" & goto :end
)

echo Generator: %GENERATOR% ^(x64^)
cmake -G "%GENERATOR%" -A x64 -B build
if errorlevel 1 (
    echo CMake generation failed.
    set "RC=1" & goto :end
)

if /i "%DOBUILD%"=="build" (
    echo Building Debug^|x64...
    cmake --build build --config Debug
    if errorlevel 1 (
        echo Build failed.
        set "RC=1" & goto :end
    )
)

if exist "build\CAFFeinated.slnx" (
    echo Solution written to build\CAFFeinated.slnx
) else if exist "build\CAFFeinated.sln" (
    echo Solution written to build\CAFFeinated.sln
) else (
    echo Generation exited with success but no solution file was found.
    set "RC=1" & goto :end
)
set "RC=0" & goto :end

:end
if not "%~1"=="" exit /b %RC%
echo.
echo Press any key to close...
pause >nul
exit /b %RC%
