
@call :GetVSCommonToolsDir
@if "%VS120COMNTOOLS%"=="" goto error_no_VS120COMNTOOLSDIR

@call "%VS120COMNTOOLS%VCVarsQueryRegistry.bat" 32bit No64bit

@if "%VSINSTALLDIR%"=="" goto error_no_VSINSTALLDIR
@if "%FrameworkDir32%"=="" goto error_no_FrameworkDIR32
@if "%FrameworkVersion32%"=="" goto error_no_FrameworkVer32
@if "%Framework40Version%"=="" goto error_no_Framework40Version

@set FrameworkDir=%FrameworkDir32%
@set FrameworkVersion=%FrameworkVersion32%

@if not "%WindowsSDK_ExecutablePath_x86%" == "" (
	@set "PATH=%WindowsSDK_ExecutablePath_x86%;%PATH%"
)

@if not "%WindowsSdkDir%" == "" (
	@set "PATH=%WindowsSdkDir%bin\x86;%PATH%"
	@set "INCLUDE=%WindowsSdkDir%include\shared;%WindowsSdkDir%include\um;%WindowsSdkDir%include\winrt;%INCLUDE%"
	@set "LIB=%WindowsSdkDir%lib\winv6.3\um\x86;%LIB%"
	@set "LIBPATH=%WindowsSdkDir%References\CommonConfiguration\Neutral;%ExtensionSDKDir%\Microsoft.VCLibs\12.0\References\CommonConfiguration\neutral;%LIBPATH%"
)

@rem
@rem Root of Visual Studio IDE installed files.
@rem
@set DevEnvDir=%VSINSTALLDIR%Common7\IDE\

@rem PATH
@rem ----
@if exist "%VSINSTALLDIR%Team Tools\Performance Tools" (
	@set "PATH=%VSINSTALLDIR%Team Tools\Performance Tools;%PATH%"
)
@if exist "%ProgramFiles%\HTML Help Workshop" set PATH=%ProgramFiles%\HTML Help Workshop;%PATH%
@if exist "%ProgramFiles(x86)%\HTML Help Workshop" set PATH=%ProgramFiles(x86)%\HTML Help Workshop;%PATH%
@if exist "%VCINSTALLDIR%VCPackages" set PATH=%VCINSTALLDIR%VCPackages;%PATH%

@if exist "%FrameworkDir%%Framework40Version%" set PATH=%FrameworkDir%%Framework40Version%;%PATH%
@if exist "%FrameworkDir%%FrameworkVersion%" set PATH=%FrameworkDir%%FrameworkVersion%;%PATH%
@if exist "%VSINSTALLDIR%Common7\Tools" set PATH=%VSINSTALLDIR%Common7\Tools;%PATH%
@if exist "%VCINSTALLDIR%BIN" set PATH=%VCINSTALLDIR%BIN;%PATH%
@set PATH=%DevEnvDir%;%PATH%

@rem Add path to MSBuild Binaries
@if exist "%ProgramFiles%\MSBuild\12.0\bin" set PATH=%ProgramFiles%\MSBuild\12.0\bin;%PATH%
@if exist "%ProgramFiles(x86)%\MSBuild\12.0\bin" set PATH=%ProgramFiles(x86)%\MSBuild\12.0\bin;%PATH%

@rem Add path to TypeScript Compiler
@if exist "%ProgramFiles%\Microsoft SDKs\TypeScript\1.0" set PATH=%ProgramFiles%\Microsoft SDKs\TypeScript\1.0;%PATH%
@if exist "%ProgramFiles(x86)%\Microsoft SDKs\TypeScript\1.0" set PATH=%ProgramFiles(x86)%\Microsoft SDKs\TypeScript\1.0;%PATH%

@if exist "%VSINSTALLDIR%VSTSDB\Deploy" (
	@set "PATH=%VSINSTALLDIR%VSTSDB\Deploy;%PATH%"
)

@if not "%FSHARPINSTALLDIR%" == "" (
	@set "PATH=%FSHARPINSTALLDIR%;%PATH%"
)

@if exist "%DevEnvDir%CommonExtensions\Microsoft\TestWindow" (
	@set "PATH=%DevEnvDir%CommonExtensions\Microsoft\TestWindow;%PATH%"
)

@rem INCLUDE
@rem -------
@if exist "%VCINSTALLDIR%ATLMFC\INCLUDE" set INCLUDE=%VCINSTALLDIR%ATLMFC\INCLUDE;%INCLUDE%
@if exist "%VCINSTALLDIR%INCLUDE" set INCLUDE=%VCINSTALLDIR%INCLUDE;%INCLUDE%

@rem LIB
@rem ---
@if exist "%VCINSTALLDIR%ATLMFC\LIB" set LIB=%VCINSTALLDIR%ATLMFC\LIB;%LIB%
@if exist "%VCINSTALLDIR%LIB" set LIB=%VCINSTALLDIR%LIB;%LIB%

@rem LIBPATH
@rem -------
@if exist "%VCINSTALLDIR%ATLMFC\LIB" set LIBPATH=%VCINSTALLDIR%ATLMFC\LIB;%LIBPATH%
@if exist "%VCINSTALLDIR%LIB" set LIBPATH=%VCINSTALLDIR%LIB;%LIBPATH%
@if exist "%FrameworkDir%%Framework40Version%" set LIBPATH=%FrameworkDir%%Framework40Version%;%LIBPATH%
@if exist "%FrameworkDir%%FrameworkVersion%" set LIBPATH=%FrameworkDir%%FrameworkVersion%;%LIBPATH%

@rem VisualStudioVersion
@rem -------------------
@set VisualStudioVersion=12.0

@goto end

@REM -----------------------------------------------------------------------
:GetVSCommonToolsDir
@set VS120COMNTOOLS=
@call :GetVSCommonToolsDirHelper32 HKLM > nul 2>&1
@if errorlevel 1 call :GetVSCommonToolsDirHelper32 HKCU > nul 2>&1
@if errorlevel 1 call :GetVSCommonToolsDirHelper64  HKLM > nul 2>&1
@if errorlevel 1 call :GetVSCommonToolsDirHelper64  HKCU > nul 2>&1
@exit /B 0

:GetVSCommonToolsDirHelper32
@for /F "tokens=1,2*" %%i in ('reg query "%1\SOFTWARE\Microsoft\VisualStudio\SxS\VS7" /v "12.0"') DO (
	@if "%%i"=="12.0" (
		@SET "VS120COMNTOOLS=%%k"
	)
)
@if "%VS120COMNTOOLS%"=="" exit /B 1
@SET "VS120COMNTOOLS=%VS120COMNTOOLS%Common7\Tools\"
@exit /B 0

:GetVSCommonToolsDirHelper64
@for /F "tokens=1,2*" %%i in ('reg query "%1\SOFTWARE\Wow6432Node\Microsoft\VisualStudio\SxS\VS7" /v "12.0"') DO (
	@if "%%i"=="12.0" (
		@SET "VS120COMNTOOLS=%%k"
	)
)
@if "%VS120COMNTOOLS%"=="" exit /B 1
@SET "VS120COMNTOOLS=%VS120COMNTOOLS%Common7\Tools\"
@exit /B 0

@REM -----------------------------------------------------------------------
:error_no_VS120COMNTOOLSDIR
@echo ERROR: Cannot determine the location of the VS Common Tools folder.
@goto end

:error_no_VSINSTALLDIR
@echo ERROR: Cannot determine the location of the VS installation.
@goto end

:error_no_FrameworkDIR32
@echo ERROR: Cannot determine the location of the .NET Framework 32bit installation.
@goto end

:error_no_FrameworkVer32
@echo ERROR: Cannot determine the version of the .NET Framework 32bit installation.
@goto end

:error_no_Framework40Version
@echo ERROR: Cannot determine the .NET Framework 4.0 version.
@goto end

:end
path=%path%;c:\apps\dos
cd \sources\version8

