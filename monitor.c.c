#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501

#include <windows.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>

// Cores para o console
#define COR_PADRAO (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define COR_VERDE (FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define COR_AMARELO (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define COR_VERMELHO (FOREGROUND_RED | FOREGROUND_INTENSITY)
#define COR_AZUL (FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define COR_CIANO (FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY)

static HANDLE hConsole;
static FILE *fp_log = NULL;
static int intervalo_ms = 1000;
static char arquivo_csv[256] = "monitor_recursos.csv";

// Definir constante se não estiver disponível
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

// Função para configurar o console para UTF-8
void configurar_console()
{
    // Configurar código de página para UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Habilitar processamento de sequências de escape ANSI (Windows 10+)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

// Alternativa 1: Usando caracteres Unicode (mais bonito)
void mostrar_cabecalho_unicode()
{
    SetConsoleTextAttribute(hConsole, COR_AZUL);
    printf("┌──────────────────────────────────────────────────────────────┐\n");
    printf("│                 MONITOR DE RECURSOS DO SISTEMA              │\n");
    printf("├──────────────────────────────────────────────────────────────┤\n");
    SetConsoleTextAttribute(hConsole, COR_PADRAO);

    char timestamp[64];
    formatar_timestamp(timestamp, sizeof(timestamp));
    printf("│ Atualizacao: %s  │  Intervalo: %4d ms     │\n", timestamp, intervalo_ms);
    printf("└──────────────────────────────────────────────────────────────┘\n\n");
}

// Alternativa 2: Usando caracteres ASCII simples (mais compatível)
void mostrar_cabecalho_ascii()
{
    SetConsoleTextAttribute(hConsole, COR_AZUL);
    printf("+--------------------------------------------------------------+\n");
    printf("|                 MONITOR DE RECURSOS DO SISTEMA              |\n");
    printf("+--------------------------------------------------------------+\n");
    SetConsoleTextAttribute(hConsole, COR_PADRAO);

    char timestamp[64];
    formatar_timestamp(timestamp, sizeof(timestamp));
    printf("| Atualizacao: %s  |  Intervalo: %4d ms     |\n", timestamp, intervalo_ms);
    printf("+--------------------------------------------------------------+\n\n");
}

// Alternativa 3: Usando caracteres de desenho estendidos (CP437/CP850)
void mostrar_cabecalho_estendido()
{
    SetConsoleTextAttribute(hConsole, COR_AZUL);
    printf("%c", 201); // ╔
    for (int i = 0; i < 62; i++)
        printf("%c", 205); // ═
    printf("%c\n", 187);   // ╗

    printf("%c                 MONITOR DE RECURSOS DO SISTEMA              %c\n", 186, 186); // ║

    printf("%c", 204); // ╠
    for (int i = 0; i < 62; i++)
        printf("%c", 205); // ═
    printf("%c\n", 185);   // ╣

    SetConsoleTextAttribute(hConsole, COR_PADRAO);

    char timestamp[64];
    formatar_timestamp(timestamp, sizeof(timestamp));
    printf("%c Atualizacao: %s  %c  Intervalo: %4d ms     %c\n",
           186, timestamp, 179, intervalo_ms, 186); // ║ │ ║

    SetConsoleTextAttribute(hConsole, COR_AZUL);
    printf("%c", 200); // ╚
    for (int i = 0; i < 62; i++)
        printf("%c", 205); // ═
    printf("%c\n\n", 188); // ╝
    SetConsoleTextAttribute(hConsole, COR_PADRAO);
}

// Menu de configurações com bordas ASCII
void mostrar_menu_config()
{
    system("cls");
    SetConsoleTextAttribute(hConsole, COR_CIANO);
    printf("+----------------------------------------+\n");
    printf("|        CONFIGURACOES DO MONITOR       |\n");
    printf("+----------------------------------------+\n\n");

    SetConsoleTextAttribute(hConsole, COR_PADRAO);
    printf("1. Alterar intervalo de atualizacao (atual: %d ms)\n", intervalo_ms);
    printf("2. Alterar arquivo de log (atual: %s)\n", arquivo_csv);
    printf("3. Voltar ao monitoramento\n");
    printf("\nEscolha uma opcao (1-3): ");
}

// Tela de boas-vindas com bordas ASCII
void mostrar_tela_inicial()
{
    system("cls");
    SetConsoleTextAttribute(hConsole, COR_VERDE);
    printf("+--------------------------------------------------------------+\n");
    printf("|                 MONITOR DE RECURSOS v2.0                    |\n");
    printf("|                      Sistema Windows                        |\n");
    printf("+--------------------------------------------------------------+\n\n");

    SetConsoleTextAttribute(hConsole, COR_PADRAO);
    printf("Carregando...\n");
    Sleep(1500);
}

// Resto das funções permanece igual...
static ULONGLONG filetime_to_ull(const FILETIME *ft)
{
    ULARGE_INTEGER li;
    li.LowPart = ft->dwLowDateTime;
    li.HighPart = ft->dwHighDateTime;
    return li.QuadPart;
}

double obter_uso_cpu()
{
    static ULONGLONG prevIdle = 0, prevTotal = 0;
    FILETIME idleFT, kernelFT, userFT;

    if (!GetSystemTimes(&idleFT, &kernelFT, &userFT))
        return -1.0;

    ULONGLONG idle = filetime_to_ull(&idleFT);
    ULONGLONG kernel = filetime_to_ull(&kernelFT);
    ULONGLONG user = filetime_to_ull(&userFT);
    ULONGLONG total = kernel + user;

    if (prevTotal != 0)
    {
        ULONGLONG totalDiff = total - prevTotal;
        ULONGLONG idleDiff = idle - prevIdle;
        double uso = 0.0;
        if (totalDiff > 0)
            uso = (double)(totalDiff - idleDiff) * 100.0 / (double)totalDiff;
        prevTotal = total;
        prevIdle = idle;
        return uso;
    }

    prevTotal = total;
    prevIdle = idle;
    return 0.0;
}

double obter_uso_memoria()
{
    MEMORYSTATUSEX m;
    m.dwLength = sizeof(m);
    if (!GlobalMemoryStatusEx(&m))
        return -1.0;

    if (m.ullTotalPhys == 0)
        return -1.0;

    double usado = (double)(m.ullTotalPhys - m.ullAvailPhys) * 100.0 / (double)m.ullTotalPhys;
    return usado;
}

int obter_dados_rede(ULONGLONG *bytesIn, ULONGLONG *bytesOut)
{
    DWORD tamanho = 0;
    DWORD resultado = GetIfTable(NULL, &tamanho, FALSE);

    if (resultado != ERROR_INSUFFICIENT_BUFFER)
        return -1;

    PMIB_IFTABLE pTabela = (PMIB_IFTABLE)malloc(tamanho);
    if (!pTabela)
        return -1;

    resultado = GetIfTable(pTabela, &tamanho, FALSE);
    if (resultado != NO_ERROR)
    {
        free(pTabela);
        return -1;
    }

    ULONGLONG totalIn = 0, totalOut = 0;
    for (DWORD i = 0; i < pTabela->dwNumEntries; ++i)
    {
        totalIn += (ULONGLONG)pTabela->table[i].dwInOctets;
        totalOut += (ULONGLONG)pTabela->table[i].dwOutOctets;
    }

    free(pTabela);
    *bytesIn = totalIn;
    *bytesOut = totalOut;
    return 0;
}

void formatar_timestamp(char *buffer, size_t tamanho)
{
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    if (tm_info)
    {
        strftime(buffer, tamanho, "%d/%m/%Y %H:%M:%S", tm_info);
    }
    else
    {
        strncpy(buffer, "N/A", tamanho);
    }
}

void formatar_bytes(double bytes, char *buffer, size_t tamanho)
{
    const char *unidades[] = {"B/s", "KB/s", "MB/s", "GB/s"};
    int indice = 0;

    while (bytes >= 1024.0 && indice < 3)
    {
        bytes /= 1024.0;
        indice++;
    }

    snprintf(buffer, tamanho, "%.2f %s", bytes, unidades[indice]);
}

void desenhar_barra(double percentual, int largura)
{
    int preenchido = (int)((percentual / 100.0) * largura);

    printf("[");
    for (int i = 0; i < largura; i++)
    {
        if (i < preenchido)
        {
            if (percentual < 50.0)
                SetConsoleTextAttribute(hConsole, COR_VERDE);
            else if (percentual < 80.0)
                SetConsoleTextAttribute(hConsole, COR_AMARELO);
            else
                SetConsoleTextAttribute(hConsole, COR_VERMELHO);
            printf("#"); // Usando # em vez de █ para melhor compatibilidade
        }
        else
        {
            SetConsoleTextAttribute(hConsole, COR_PADRAO);
            printf("-");
        }
    }
    SetConsoleTextAttribute(hConsole, COR_PADRAO);
    printf("] %.1f%%", percentual);
}

void processar_configuracoes()
{
    int opcao;
    scanf("%d", &opcao);

    switch (opcao)
    {
    case 1:
        printf("\nDigite o novo intervalo em milissegundos (min. 100): ");
        scanf("%d", &intervalo_ms);
        if (intervalo_ms < 100)
            intervalo_ms = 100;
        printf("Intervalo alterado para %d ms\n", intervalo_ms);
        break;

    case 2:
        printf("\nDigite o nome do novo arquivo CSV: ");
        scanf("%255s", arquivo_csv);
        printf("Arquivo alterado para %s\n", arquivo_csv);
        break;

    case 3:
        return;

    default:
        printf("Opcao invalida!\n");
    }

    printf("\nPressione qualquer tecla para continuar...");
    _getch();
}

void mostrar_info_sistema()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    MEMORYSTATUSEX mem;
    mem.dwLength = sizeof(mem);
    GlobalMemoryStatusEx(&mem);

    printf("INFORMACOES DO SISTEMA:\n");
    printf("  Processadores: %lu\n", (unsigned long)si.dwNumberOfProcessors);
    printf("  Memoria Total: %.1f GB\n", (double)mem.ullTotalPhys / (1024 * 1024 * 1024));
    printf("  Pagina Total:  %.1f GB\n\n", (double)mem.ullTotalPageFile / (1024 * 1024 * 1024));
}

void executar_monitoramento()
{
    fp_log = fopen(arquivo_csv, "a+");
    if (fp_log)
    {
        fseek(fp_log, 0, SEEK_END);
        if (ftell(fp_log) == 0)
        {
            fprintf(fp_log, "timestamp,cpu_percent,mem_percent,net_in_Bps,net_out_Bps\n");
            fflush(fp_log);
        }
    }

    obter_uso_cpu();
    ULONGLONG prevIn = 0, prevOut = 0;
    obter_dados_rede(&prevIn, &prevOut);

    printf("Monitor iniciado! Pressione 'c' para configuracoes ou 'q' para sair.\n");
    Sleep(2000);

    while (1)
    {
        if (_kbhit())
        {
            char tecla = _getch();
            if (tecla == 'q' || tecla == 'Q')
            {
                break;
            }
            else if (tecla == 'c' || tecla == 'C')
            {
                mostrar_menu_config();
                processar_configuracoes();
            }
        }

        system("cls");

        // Use uma das três opções:
        mostrar_cabecalho_ascii(); // Mais compatível
        // mostrar_cabecalho_unicode();  // Mais bonito (descomente se quiser tentar)
        // mostrar_cabecalho_estendido(); // Intermediário

        double cpu = obter_uso_cpu();
        double memoria = obter_uso_memoria();

        ULONGLONG curIn = 0, curOut = 0;
        obter_dados_rede(&curIn, &curOut);

        ULONGLONG deltaIn = (curIn >= prevIn) ? (curIn - prevIn) : 0;
        ULONGLONG deltaOut = (curOut >= prevOut) ? (curOut - prevOut) : 0;

        double intervalos = intervalo_ms / 1000.0;
        double velocIn = deltaIn / intervalos;
        double velocOut = deltaOut / intervalos;

        SetConsoleTextAttribute(hConsole, COR_CIANO);
        printf("CPU:     ");
        desenhar_barra(cpu, 30);
        printf("\n");

        SetConsoleTextAttribute(hConsole, COR_CIANO);
        printf("MEMORIA: ");
        desenhar_barra(memoria, 30);
        printf("\n\n");

        SetConsoleTextAttribute(hConsole, COR_VERDE);
        char bufferIn[32], bufferOut[32];
        formatar_bytes(velocIn, bufferIn, sizeof(bufferIn));
        formatar_bytes(velocOut, bufferOut, sizeof(bufferOut));

        printf("REDE:\n");
        printf("  Download: %12s\n", bufferIn);
        printf("  Upload:   %12s\n\n", bufferOut);

        SetConsoleTextAttribute(hConsole, COR_PADRAO);
        mostrar_info_sistema();

        if (fp_log)
        {
            char timestamp[64];
            formatar_timestamp(timestamp, sizeof(timestamp));
            fprintf(fp_log, "%s,%.2f,%.2f,%.0f,%.0f\n",
                    timestamp, cpu, memoria, velocIn, velocOut);
            fflush(fp_log);
        }

        SetConsoleTextAttribute(hConsole, COR_AMARELO);
        printf("CONTROLES: [C] Configuracoes  [Q] Sair\n");
        SetConsoleTextAttribute(hConsole, COR_PADRAO);

        prevIn = curIn;
        prevOut = curOut;

        Sleep(intervalo_ms);
    }

    if (fp_log)
    {
        fclose(fp_log);
        fp_log = NULL;
    }
}

int main(int argc, char *argv[])
{
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTitle("Monitor de Recursos do Sistema");

    // Tentar configurar UTF-8 (descomente se quiser tentar)
    // configurar_console();

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        fprintf(stderr, "Erro ao inicializar Winsock.\n");
        return 1;
    }

    if (argc >= 2)
    {
        intervalo_ms = atoi(argv[1]);
        if (intervalo_ms < 100)
            intervalo_ms = 100;
    }
    if (argc >= 3)
    {
        strncpy(arquivo_csv, argv[2], sizeof(arquivo_csv) - 1);
        arquivo_csv[sizeof(arquivo_csv) - 1] = '\0';
    }

    mostrar_tela_inicial();
    executar_monitoramento();

    WSACleanup();
    SetConsoleTextAttribute(hConsole, COR_VERDE);
    printf("\nObrigado por usar o Monitor de Recursos!\n");
    SetConsoleTextAttribute(hConsole, COR_PADRAO);

    return 0;
}