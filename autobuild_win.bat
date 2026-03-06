@echo off
setlocal enabledelayedexpansion

:: Define the build directory path (Windows style)
set "BUILD_DIR=%USERPROFILE%\Documents\CtrlrX\build"

echo Select build type:
echo 1) Full Release (Clean + CMake)
echo 2) Full Debug (Clean + CMake)
echo 3) Incremental Rebuild (Clean + Build)
echo 4) Quick Rebuild (Build only)

set /p choice="Enter choice [1-4]: "

if "%choice%"=="1" goto RELEASE
if "%choice%"=="2" goto DEBUG
if "%choice%"=="3" goto INCREMENTAL
if "%choice%"=="4" goto QUICK
echo Invalid selection. Exiting.
exit /b 1

:RELEASE
if exist "%BUILD_DIR%" rd /s /q "%BUILD_DIR%"
mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%" || exit /b
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release --parallel %NUMBER_OF_PROCESSORS%
goto END

:DEBUG
if exist "%BUILD_DIR%" rd /s /q "%BUILD_DIR%"
mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%" || exit /b
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug --parallel %NUMBER_OF_PROCESSORS%
goto END

:INCREMENTAL
if not exist "%BUILD_DIR%" (
    echo Build dir not found. Run 1 or 2 first.
    exit /b 1
)
cd /d "%BUILD_DIR%"
cmake --build . --target clean
cmake --build . --parallel %NUMBER_OF_PROCESSORS%
goto END

:QUICK
if not exist "%BUILD_DIR%" (
    echo Build dir not found. Run 1 or 2 first.
    exit /b 1
)
cd /d "%BUILD_DIR%"
cmake --build . --parallel %NUMBER_OF_PROCESSORS%
goto END

:END
echo Done!
pause