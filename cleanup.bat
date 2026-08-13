@echo off
setlocal EnableDelayedExpansion
cd /d "%~dp0"

::Safeguard to make sure this is the CAFFeinated root
if not exist ".git" goto :not_repo
if not exist "CMakeLists.txt" goto :not_repo
if not exist "code\" goto :not_repo

set "MODE=ask"
if /i "%~1"=="/y" set "MODE=force"
if /i "%~1"=="-y" set "MODE=force"
if /i "%~1"=="/n" set "MODE=dry"
if /i "%~1"=="-n" set "MODE=dry"

set "TARGET_DIRS=.vs build x64 Win32 x86 CAFFeinated CMakeFiles"
set "TARGET_FILES=imgui.ini CMakeCache.txt cmake_install.cmake"
set "TARGET_GLOBS=*.vcxproj *.vcxproj.filters *.vcxproj.user *.sln *.slnx *.lnk"

set "FOUND=0"
echo.
echo The following generated items were found:
echo.

for %%D in (%TARGET_DIRS%) do (
    if exist "%%D\" (
        echo [dir] %%D\
        set /a FOUND+=1
    )
)
for %%F in (%TARGET_FILES%) do (
    if exist "%%F" (
        echo [file] %%F
        set /a FOUND+=1
    )
)
for %%F in (%TARGET_GLOBS%) do (
    if exist "%%F" (
        echo [file] %%F
        set /a FOUND+=1
    )
)

if "!FOUND!"=="0" (
    echo    ^(nothing, already clean^)
    set "RC=0" & goto :end
)

echo.
if "%MODE%"=="dry" (
    echo Nothing was removed since this was a dry run. Rerun without /n to delete.
    set "RC=0" & goto :end
)

if "%MODE%"=="ask" (
    set "ANSWER="
    set /p "ANSWER=Remove these !FOUND! items? [y/N] "
    if /i not "!ANSWER!"=="y" (
        echo Nothing was removed.
        set "RC=0" & goto :end
    )
    echo.
)

set "FAILED=0"

for %%D in (%TARGET_DIRS%) do (
    if exist "%%D\" (
        rmdir /s /q "%%D" 2>nul
        if exist "%%D\" (
            echo [Failed] Visual Studio might be running in the background. Close it and rerun cleanup.bat.
            set /a FAILED+=1
        )
    )
)
for %%F in (%TARGET_FILES%) do (
    if exist "%%F" (
        del /f /q "%%F" 2>nul
        if exist "%%F" ( echo [Failed] %%F & set /a FAILED+=1 )
    )
)
for %%F in (%TARGET_GLOBS%) do (
    if exist "%%F" (
        del /f /q "%%F" 2>nul
        if exist "%%F" ( echo [Failed] %%F & set /a FAILED+=1 )
    )
)

echo.
if not "!FAILED!"=="0" (
    echo !FAILED! items could not be removed.
    set "RC=1" & goto :end
)
echo Project is now clean. Run generate.bat to regenerate the CMake solution.
set "RC=0" & goto :end

:not_repo
echo This does not look like the CAFFeinated repository root.
set "RC=1" & goto :end

:end
if not "%~1"=="" exit /b %RC%
echo.
echo Press any key to close...
pause >nul
exit /b %RC%