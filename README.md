# 🖥️ Monitor de Recursos do Sistema

Um monitor de recursos em tempo real para sistemas Windows, desenvolvido em C, que exibe informações sobre CPU, memória e rede de forma visual e interativa.



## ✨ Características

- **Monitor em tempo real** de CPU, memória e tráfego de rede
- **Interface colorida** no terminal com barras de progresso
- **Logging automático** em arquivo CSV para análise posterior
- **Configurações personalizáveis** via menu interativo
- **Compatível** com diferentes versões do Windows
- **Interface limpa** com bordas ASCII bem formatadas

## 🚀 Compilação e Execução

### Pré-requisitos
- **Compilador GCC** (MinGW no Windows)
- **Windows SDK** ou bibliotecas do Windows
- **Terminal** com suporte a cores

### Compilar
```bash
gcc -o monitor.exe monitor.c -lws2_32 -liphlpapi
```

### Executar
```bash
# Execução padrão (intervalo de 1 segundo)
./monitor.exe

# Personalizar intervalo e arquivo de log
./monitor.exe 2000 meu_log.csv
```

## 🎮 Controles

Durante a execução:
- **`C`** - Abrir menu de configurações
- **`Q`** - Sair do programa

### Menu de Configurações
1. Alterar intervalo de atualização (mínimo 100ms)
2. Alterar nome do arquivo de log
3. Voltar ao monitoramento

## 📊 Dados Coletados

O programa monitora e salva no CSV:
- **CPU**: Percentual de uso do processador
- **Memória**: Percentual de uso da RAM
- **Rede**: Velocidade de download e upload em bytes/segundo
- **Timestamp**: Data e hora de cada medição

### Formato do CSV
```csv
timestamp,cpu_percent,mem_percent,net_in_Bps,net_out_Bps
09/08/2025 01:44:48,4.20,39.20,0,0
```

## 🔧 Funcionalidades

### Monitor Visual
- **Barras de progresso coloridas**:
  - 🟢 Verde: 0-50% (uso normal)
  - 🟡 Amarelo: 50-80% (uso moderado)
  - 🔴 Vermelho: 80-100% (uso alto)

### Informações do Sistema
- Número de processadores
- Memória total instalada
- Tamanho total do arquivo de paginação

### Rede
- Monitoramento de todas as interfaces de rede
- Velocidades formatadas automaticamente (B/s, KB/s, MB/s, GB/s)

## 📁 Estrutura do Projeto

```
monitor-recursos/
├── monitor.c           # Código fonte principal
├── README.md          # Este arquivo
├── LICENSE           # Licença MIT
├── .gitignore        # Arquivos a serem ignorados
├── screenshot.png    # Captura de tela
└── exemplo_log.csv   # Exemplo de arquivo de log
```

## 🛠️ Personalização

### Cores
As cores podem ser modificadas nas constantes no início do código:
```c
#define COR_VERDE (FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define COR_AMARELO (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
// ...
```

### Intervalo Padrão
Altere a variável `intervalo_ms` para mudar o intervalo padrão:
```c
static int intervalo_ms = 1000; // 1 segundo
```

## 🐛 Solução de Problemas

### Caracteres estranhos nas bordas
O programa usa bordas ASCII simples para máxima compatibilidade. Se ainda houver problemas, descomente a função `configurar_console()` no `main()`.

### Erro de compilação
Certifique-se de incluir as bibliotecas necessárias:
```bash
gcc -o monitor.exe monitor.c -lws2_32 -liphlpapi -static
```

### Permissões de rede
Algumas informações de rede podem requerer execução como administrador.

## 📈 Exemplo de Uso

1. **Monitoramento básico**: Execute o programa e observe as métricas em tempo real
2. **Análise de performance**: Use o arquivo CSV gerado para analisar padrões de uso
3. **Monitoramento prolongado**: Configure intervalos maiores para monitoramento de longo prazo
4. **Debugging**: Use para identificar picos de CPU ou vazamentos de memória

## 🤝 Contribuições

Contribuições são bem-vindas! Sinta-se à vontade para:
- Reportar bugs
- Sugerir melhorias
- Enviar pull requests
- Melhorar a documentação

### Como Contribuir
1. Faça um fork do projeto
2. Crie uma branch para sua feature (`git checkout -b feature/nova-funcionalidade`)
3. Commit suas mudanças (`git commit -am 'Adiciona nova funcionalidade'`)
4. Push para a branch (`git push origin feature/nova-funcionalidade`)
5. Abra um Pull Request

## 📄 Licença

Este projeto está sob a licença MIT. Veja o arquivo [LICENSE](LICENSE) para mais detalhes.

## 🏆 Créditos

Desenvolvido com ❤️ para monitoramento eficiente de recursos do sistema Windows.

## 📸 Captura de Tela

Visualização do Monitor de Recursos do Sistema em execução:

<img width="872" height="458" alt="Monitor exeuc" src="https://github.com/user-attachments/assets/c11cf047-4b59-41b9-afa3-332fefadd375" />


**Nota**: Este projeto foi desenvolvido e testado no Windows. Para outras plataformas, adaptações podem ser necessárias.
