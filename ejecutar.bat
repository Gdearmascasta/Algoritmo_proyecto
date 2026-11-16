@echo off
rem Compila (si hace falta) y abre el programa de comparacion en Windows.
rem Uso: doble clic en este archivo          -> menu interactivo
rem      ejecutar.bat --reporte 10000        -> los 3 escenarios sin menu
setlocal
cd /d "%~dp0"
chcp 65001 > nul

rem Recompilar si no existe el .exe o si algun archivo de src es mas nuevo
set COMPILAR=0
if not exist comparar.exe set COMPILAR=1
if exist comparar.exe (
    powershell -NoProfile -Command "$exe = (Get-Item 'comparar.exe').LastWriteTime; if (Get-ChildItem 'src' | Where-Object { $_.LastWriteTime -gt $exe }) { exit 1 } else { exit 0 }"
    if errorlevel 1 set COMPILAR=1
)

if "%COMPILAR%"=="1" (
    where g++ > nul 2>&1
    if not errorlevel 1 (
        echo Compilando con g++...
        g++ -std=c++17 -O2 -Wl,--stack,16777216 -o comparar.exe src\main.cpp
        if errorlevel 1 goto error_compilacion
    ) else (
        where cl > nul 2>&1
        if not errorlevel 1 (
            echo Compilando con Visual C++...
            cl /nologo /std:c++17 /O2 /EHsc /utf-8 /F 16777216 src\main.cpp /Fe:comparar.exe > nul
            if errorlevel 1 goto error_compilacion
            del /q main.obj 2> nul
        ) else (
            goto sin_compilador
        )
    )
)

comparar.exe %*
goto fin

:sin_compilador
echo.
echo No se encontro un compilador de C++.
echo Opciones:
echo   1. MSYS2 / MinGW-w64: instala g++ y agrega su carpeta bin al PATH
echo      (por ejemplo C:\msys64\ucrt64\bin).
echo   2. Visual Studio: abre "Developer Command Prompt" y ejecuta este .bat desde ahi.
goto fin

:error_compilacion
echo.
echo La compilacion fallo. Revisa los mensajes de arriba.

:fin
echo.
pause
endlocal
