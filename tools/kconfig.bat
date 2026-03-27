@echo off

rem tools/kconfig.bat
rem
rem SPDX-License-Identifier: Apache-2.0
rem
rem Licensed to the Apache Software Foundation (ASF) under one or more
rem contributor license agreements.  See the NOTICE file distributed with
rem this work for additional information regarding copyright ownership.  The
rem ASF licenses this file to you under the Apache License, Version 2.0 (the
rem "License"); you may not use this file except in compliance with the
rem License.  You may obtain a copy of the License at
rem
rem   http://www.apache.org/licenses/LICENSE-2.0
rem
rem Unless required by applicable law or agreed to in writing, software
rem distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
rem WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
rem License for the specific language governing permissions and limitations
rem under the License.
rem

rem Remember the state of the PATH variable on entry

set oldpath=%PATH%

rem Handle command line options

set action=%1
shift
if "%action%"=="" goto :MissingArgument

set appsdir=..\apps
set cygwindir=
set msysdir=

:ArgLoop

if "%1"=="" goto :CheckArguments

if "%1"=="-a" (
  shift
  set appsdir=%1
  goto :NextArg
)

if "%1"=="-c" (
  shift
  set cygwindir=%1
  goto :NextArg
)

if "%1"=="-m" (
  shift
  set msysdir=%1
  goto :NextArg
)

echo ERROR: Unrecognized option: %1
goto :ShowUsage

:NextArg
shift
goto :ArgLoop

rem Verify that all of the paths are valid

:CheckArguments
if exist "%appsdir%" goto :SetPath

echo ERROR: %appsdir% does not exist
goto :ShowUsage

rem Setup some required environment variables and PATH settings
rem Priority: explicit -c/-m argument > auto-detect MSYS2 > auto-detect Cygwin > use PATH as-is

:SetPath
set APPSDIR=%appsdir%

if not "%cygwindir%"=="" (
  if not exist "%cygwindir%" (
    echo ERROR: %cygwindir% does not exist
    goto :ShowUsage
  )
  set PATH=%cygwindir%\usr\local\bin;%cygwindir%\usr\bin;%cygwindir%\bin;%PATH%
  goto :DoAction
)

if not "%msysdir%"=="" (
  if not exist "%msysdir%" (
    echo ERROR: %msysdir% does not exist
    goto :ShowUsage
  )
  set PATH=%msysdir%\usr\bin;%msysdir%\bin;%PATH%
  goto :DoAction
)

rem No explicit path given; check well-known default locations
if exist "C:\msys64\usr\bin\kconfig-conf.exe" (
  set PATH=C:\msys64\usr\bin;C:\msys64\bin;%PATH%
  goto :DoAction
)

if exist "C:\msys32\usr\bin\kconfig-conf.exe" (
  set PATH=C:\msys32\usr\bin;C:\msys32\bin;%PATH%
  goto :DoAction
)

if exist "C:\Cygwin\bin\kconfig-conf.exe" (
  set PATH=C:\Cygwin\usr\local\bin;C:\Cygwin\usr\bin;C:\Cygwin\bin;%PATH%
  goto :DoAction
)

if exist "C:\cygwin64\bin\kconfig-conf.exe" (
  set PATH=C:\cygwin64\usr\local\bin;C:\cygwin64\usr\bin;C:\cygwin64\bin;%PATH%
  goto :DoAction
)

rem Last resort: check if kconfig tools are already in PATH
where kconfig-conf.exe >nul 2>&1
if not errorlevel 1 goto :DoAction

echo ERROR: Cannot find kconfig tools (kconfig-conf.exe / kconfig-mconf.exe).
echo kconfig-conf / kconfig-mconf are POSIX tools and are not available for MSVC.
echo.
echo Options:
echo   1. Install MSYS2 from https://www.msys2.org and run:
echo        pacman -S kconfig-frontends
echo      then pass: %0 %action% -m C:\msys64
echo.
echo   2. Pass the Cygwin installation directory explicitly:
echo        %0 %action% -c ^<cygwindir^>
echo.
echo   3. Use the CMake-based workflow instead (no MSYS2/Cygwin required,
echo      works with MSVC or any toolchain):
echo.
echo        pip install kconfiglib
echo.

where cl.exe >nul 2>&1
if not errorlevel 1 (
  echo        cmake -B build -DBOARD_CONFIG=^<board^>:^<config^> -G"Visual Studio 17 2022" -A x64
  echo        cmake --build build --target menuconfig
  echo.
  echo        ^(menuconfig uses Python guiconfig/Tkinter on Windows -- no windows-curses needed^)
) else (
  echo        cmake -B build -DBOARD_CONFIG=^<board^>:^<config^> -GNinja
  echo        cmake --build build --target menuconfig
  echo.
  echo        ^(menuconfig uses Python guiconfig/Tkinter on Windows -- no windows-curses needed^)
)
goto End

:DoAction
if "%action%"=="config" goto :DoConfig
if "%action%"=="oldconfig" goto :DoOldConfig
if "%action%"=="menuconfig" goto :DoMenuConfig

echo ERROR: Unrecognized action: %action%
goto :ShowUsage

:DoConfig
kconfig-conf Kconfig
goto End

:DoOldConfig
kconfig-conf --oldconfig Kconfig
goto End

:DoMenuConfig
kconfig-mconf Kconfig
goto End

:MissingArgument

echo ERROR: Missing required argument

:ShowUsage
echo USAGE: %0 ^<action^> [-a ^<appsdir^>] [-c ^<cygwindir^>] [-m ^<msysdir^>]
echo Where:
echo  ^<action^> is one of config, oldconfig, or menuconfig
echo  ^<appsdir^> is the relative path to the apps\ directory.
echo    This defaults to ..\apps
echo  ^<cygwindir^> is an optional Cygwin installation directory. If specified,
echo    the path must exist. If omitted, MSYS2/Cygwin is auto-detected or
echo    the existing PATH is used.
echo  ^<msysdir^> is an optional MSYS2 installation directory. If specified,
echo    the path must exist.
echo.
echo  If no MSYS2 or Cygwin is installed, use the CMake-based workflow:
echo    pip install kconfiglib
echo    cmake -B build -DBOARD_CONFIG=^<board^>:^<config^>
echo    cmake --build build --target menuconfig
echo  ^(windows-curses is NOT needed for the CMake guiconfig path^)

rem Restore the original PATH settings

:End
set PATH=%oldpath%
