@echo off
setlocal enabledelayedexpansion
:: #To build with LuaJIT: cmake -B build -DCTRLRX_USE_LUAJIT=ON
:: #To build without (default): cmake -B build
:: #Check whether you have Ninja installed and available in PATH, as it will speed up the build significantly. 
:: #If not, you can install it via your system package manager or from https://ninja-build.org/. 
:: #On Windows, you can also install it via winget:
:: #ninja --version
:: #winget install -e --id Ninja-build.Ninja
:: #cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCTRLRX_USE_LUAJIT=ON ..
:: #cmake --build build
set "BUILD_DIR=%USERPROFILE%\Documents\CtrlrX\build"
set "PROCESSORS=%PROCESSORS%"

::==============================================================================
:: Bootstrap VS environment if cl.exe isn't already available
::==============================================================================
where cl >nul 2>&1
if errorlevel 1 (
    echo Initialising Visual Studio environment...
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
)

::==============================================================================
:: Verify Ninja is available
::==============================================================================
where ninja >nul 2>&1
if errorlevel 1 (
    echo ERROR: Ninja not found on PATH. Please install it and try again.
    pause
    exit /b 1
)

::==============================================================================
:: Build LuaJIT if source exists but lib is missing
:: If luajit source is not present (non-LuaJIT branch) this is silently skipped
::==============================================================================
set "LUAJIT_FLAG="
echo PATH CHECK: "%~dp0Source\Misc\luajit\src\msvcbuild.bat"
if exist "%~dp0Source\Misc\luajit\src\msvcbuild.bat" (
    set "LUAJIT_FLAG=-DCTRLRX_USE_LUAJIT=ON"
) else (
    echo LuaJIT source not found - building without LuaJIT.
)

::==============================================================================
:: Prompt: Build Type
::==============================================================================
echo.
echo  [1] Release
echo  [2] Debug
echo  [3] RelWithDebInfo  - Release speed, no LTO optimisation (fast compile, works in DAW)
echo.
set /p CONFIG_CHOICE="Build configuration [1-2]: "

if "%CONFIG_CHOICE%"=="1" (
    set "CONFIG=Release"
) else if "%CONFIG_CHOICE%"=="2" (
    set "CONFIG=Debug"
) else if "%CONFIG_CHOICE%"=="3" (
    set "CONFIG=RelWithDebInfo"
) else (
    echo Invalid selection. Exiting.
    exit /b 1
)

::==============================================================================
:: Prompt: Build Mode
::==============================================================================
echo.

echo  [1] Full Build     - Wipe build dir, run CMake, build
echo  [2] Clean Build    - Keep CMake cache and Ninja files, wipe objects only
echo  [3] Quick Build    - Incremental, no clean
echo.
set /p MODE_CHOICE="Build mode [1-3]: "
taskkill /f /im CtrlrX.exe 2>nul
if "%MODE_CHOICE%"=="1" goto FULL
if "%MODE_CHOICE%"=="2" goto CLEAN
if "%MODE_CHOICE%"=="3" goto QUICK
echo Invalid selection. Exiting.
exit /b 1

::==============================================================================
:FULL
::==============================================================================

echo.
echo [FULL BUILD] Config: %CONFIG%
if defined LUAJIT_FLAG (
    echo LuaJIT: ENABLED
) else (
    echo LuaJIT: DISABLED
)
echo Wiping build directory...
if exist "%BUILD_DIR%" rd /s /q "%BUILD_DIR%"
:WAIT_DELETE
if exist "%BUILD_DIR%" (
    echo Waiting for directory release...
    timeout /t 1 /nobreak >nul
    goto WAIT_DELETE
)
mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%" || exit /b 1

echo Running CMake configure (Ninja)...
cmake -G "Ninja" ^
  -DCMAKE_C_COMPILER=cl ^
  -DCMAKE_CXX_COMPILER=cl ^
  -DCMAKE_BUILD_TYPE=%CONFIG% ^
  %LUAJIT_FLAG% ^
  -DCMAKE_EXE_LINKER_FLAGS_DEBUG="/incremental" ^
  -DCMAKE_SHARED_LINKER_FLAGS_DEBUG="/incremental" ^
  -DCMAKE_EXE_LINKER_FLAGS_RELEASE="/LTCG" ^
  -DCMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO="" ^
  -DCMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO="" ^
  -DCMAKE_SHARED_LINKER_FLAGS_RELEASE="/LTCG" ^
  .. || goto ERROR

echo Building...
:: cmake --build . --parallel %PROCESSORS% || goto ERROR
cmake --build . --parallel %PROCESSORS% > build_log.txt 2>&1 || goto ERROR
goto END

::==============================================================================
:CLEAN
::==============================================================================
echo.
echo View compiler messages in build/build_log.txt
echo [CLEAN BUILD] Config: %CONFIG%
if not exist "%BUILD_DIR%" (
    echo Build directory not found - run a Full Build first.
    exit /b 1
)
cd /d "%BUILD_DIR%" || exit /b 1

echo Wiping compiled objects...
if exist "%BUILD_DIR%\CMakeFiles"  rd /s /q "%BUILD_DIR%\CMakeFiles"
del /s /q "%BUILD_DIR%\*.obj"     2>nul
del /s /q "%BUILD_DIR%\*.ilk"     2>nul
del /s /q "%BUILD_DIR%\*.pdb"     2>nul

echo Building...
:: cmake --build . --parallel %PROCESSORS% || goto ERROR
cmake --build . --parallel %PROCESSORS% > build_log.txt 2>&1 || goto ERROR
goto END

::==============================================================================
:QUICK
::==============================================================================
echo.
echo [QUICK BUILD] Config: %CONFIG%
if not exist "%BUILD_DIR%" (
    echo Build directory not found - run a Full Build first.
    exit /b 1
)
cd /d "%BUILD_DIR%" || exit /b 1

echo Building...
:: cmake --build . --parallel %PROCESSORS% || goto ERROR
cmake --build . --parallel %PROCESSORS% > build_log.txt 2>&1 || goto ERROR
goto END

::==============================================================================
:ERROR
echo.
echo *** BUILD FAILED ***
pause
exit /b 1

:END
echo.
echo Build complete! [%CONFIG%]
pause