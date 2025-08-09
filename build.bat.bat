@echo off
echo Compilando Monitor de Recursos do Sistema...
echo.

REM Verificar se o GCC está instalado
gcc --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERRO: GCC não encontrado. Instale o MinGW-w64 primeiro.
    echo Download: https://www.mingw-w64.org/downloads/
    pause
    exit /b 1
)

REM Compilar o programa
gcc -o monitor.exe monitor.c -lws2_32 -liphlpapi -static

if %errorlevel% equ 0 (
    echo.
    echo ✓ Compilacao concluida com sucesso!
    echo ✓ Executavel criado: monitor.exe
    echo.
    echo Para executar:
    echo   monitor.exe
    echo   ou
    echo   monitor.exe [intervalo_ms] [arquivo_log.csv]
    echo.
    echo Exemplo:
    echo   monitor.exe 2000 meu_log.csv
) else (
    echo.
    echo ✗ Erro durante a compilacao!
    echo Verifique se todas as dependencias estao instaladas.
)

echo.
pause