#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <csignal>
#include <sstream>
#include <sys/wait.h>
#include <fstream> // Necessário para salvar o arquivo de log

using namespace std;

const string RESET     = "\033[0m";
const string ROXO      = "\033[35m\033[1m";
const string VERMELHO  = "\033[31m\033[1m";
const string VERDE     = "\033[32m\033[1m";
const string CIANO     = "\033[36m\033[1m";
const string AMARELO   = "\033[33m\033[1m";
const string BRANCO    = "\033[37m\033[1m";
const string PISCANDO  = "\033[5m";

int socket_principal;

void gangster_shutdown(int sinal) {
    cout << "\n\n" << VERMELHO << "[-] [SISTEMA] Desconectando módulos e liberando portas..." << RESET << endl;
    close(socket_principal);
    exit(sinal);
}

void limpar_processos_filhos(int sinal) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
    signal(SIGINT, gangster_shutdown);
    signal(SIGCHLD, limpar_processos_filhos);
    signal(SIGPIPE, SIG_IGN); 
    
    system("clear");

    int PORTA_ARMADILHA       = 8081;          
    int LINHAS_DE_INUNDACAO   = 100000; 
    string NOME_SERVIDOR_FAKE = "Nginx/1.24.0 (Ubuntu)"; 
    string VENENO_TERMINAL    = "🚨 [ZODIAC BY NULL] -> OVERFLOW_CRITICAL_WARNING: SYSTEM_HIERARCHY_COMPROMISED_🚨\n";

    cout << ROXO << "=================================================================================" << RESET << endl;
    cout << CIANO << " ______  ____  _____   _    ____ " << endl;
    cout << CIANO << "|_  _  |/ __ \\|  __ \\ | |  / ___|" << endl;
    cout << CIANO << "  / /  | /  \\ | |  \\ \\| | | |      " << VERMELHO << "   [ OPERADOR SUPREMO ]" << endl;
    cout << CIANO << " / /__ | \\__/ | |__/ /| | | |___   " << BRANCO << "       ⚡ " << VERMELHO << PISCANDO << "N U L L" << RESET << BRANCO << " ⚡" << endl;
    cout << CIANO << "|_____| \\____/|_____/ |_|  \\____|  " << VERDE << "   (🔥 ANALISTA DE AMEAÇAS 🔥)" << endl;
    cout << ROXO << "=================================================================================" << RESET << endl;
    cout << VERDE << "   [+] Sensores ativos e analíticos linkados na porta: " << BRANCO << PORTA_ARMADILHA << RESET << endl;
    cout << CIANO << "   [*] Mapeamento: Windows, Linux, macOS e detecção de Payloads." << RESET << endl;
    cout << ROXO << "=================================================================================" << RESET << endl;
    cout << AMARELO << "   [*] Escudo ativado. Vigilância passiva rodando em background...\n" << RESET << endl;

    socket_principal = socket(AF_INET, SOCK_STREAM, 0);
    int op = 1;
    setsockopt(socket_principal, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));

    struct sockaddr_in endereco;
    memset(&endereco, 0, sizeof(endereco));
    endereco.sin_family = AF_INET;
    endereco.sin_addr.s_addr = INADDR_ANY;
    endereco.sin_port = htons(PORTA_ARMADILHA);

    if (bind(socket_principal, (struct sockaddr*)&endereco, sizeof(endereco)) < 0) {
        cout << VERMELHO << "[!] ERRO CRÍTICO: A porta " << PORTA_ARMADILHA << " está bloqueada." << RESET << endl;
        return 1;
    }

    listen(socket_principal, 100);

    bool ZODIAC_NULL_OVERSIZE_PROTECTION = true;

    while (ZODIAC_NULL_OVERSIZE_PROTECTION) {
        struct sockaddr_in cliente;
        socklen_t tamanho_addr = sizeof(cliente);
        int socket_alvo = accept(socket_principal, (struct sockaddr*)&cliente, &tamanho_addr);

        if (socket_alvo >= 0) {
            string ip_invasor = inet_ntoa(cliente.sin_addr);
            
            pid_t pid = fork();
            
            if (pid == 0) {
                close(socket_principal); 
                
                char buffer_requisicao[2048] = {0};
                recv(socket_alvo, buffer_requisicao, sizeof(buffer_requisicao) - 1, 0);
                string dados_requisicao(buffer_requisicao);

                // 1. RASTREAMENTO DO SISTEMA OPERACIONAL (Fingerprinting)
                string os_detectado = "Desconhecido / Outro";
                string dados_lower = dados_requisicao;
                for (auto &c : dados_lower) c = tolower(c);

                if (dados_lower.find("windows") != string::npos) {
                    os_detectado = "Windows PC OS 🪟";
                } else if (dados_lower.find("macintosh") != string::npos || dados_lower.find("mac os") != string::npos || dados_lower.find("iphone") != string::npos) {
                    os_detectado = "macOS / iOS Apple 🍏";
                } else if (dados_lower.find("linux") != string::npos) {
                    os_detectado = "Linux Kernel OS 🐧";
                }

                // 2. DETECÇÃO DE PAYLOADS E FERRAMENTAS INVASORAS
                string tipo_requisicao = "Acesso Web Convencional";
                string severidade = VERDE + "Baixa (Vulnerabilidade Não Explorada)" + RESET;
                
                if (dados_lower.find("nmap") != string::npos) {
                    tipo_requisicao = "Varredura de Portas (Nmap Scanner)";
                    severidade = AMARELO + "Média (Reconhecimento Ativo)" + RESET;
                } else if (dados_lower.find("sqlmap") != string::npos || dados_lower.find("select") != string::npos || dados_lower.find("union") != string::npos) {
                    tipo_requisicao = "Tentativa de Injeção SQL (Exploit)";
                    severidade = VERMELHO + PISCANDO + "ALTA (Ataque Crítico Detectado!)" + RESET;
                } else if (dados_lower.find("python") != string::npos || dados_lower.find("curl") != string::npos) {
                    tipo_requisicao = "Script Automatizado Automatizado";
                    severidade = AMARELO + "Média (Script de Varredura)" + RESET;
                }

                // 3. GRAVAÇÃO AUTOMÁTICA NO ARQUIVO DE LOG DE INTELIGÊNCIA
                ofstream arquivo_log("zodiac_threat_intelligence.log", ios::app);
                if (arquivo_log.is_open()) {
                    time_t agora = time(0);
                    char* hora_texto = ctime(&agora);
                    hora_texto[strcspn(hora_texto, "\n")] = 0;
                    
                    arquivo_log << "[" << hora_texto << "] IP: " << ip_invasor 
                                << " | OS: " << os_detectado 
                                << " | Tipo: " << tipo_requisicao << "\n";
                    arquivo_log.close();
                }

                // Exibe os metadados mastigados na tela do console principal
                cout << VERMELHO << "🚨 [ALVO TOTALMENTE MAPEADO] ➔ ASSINATURA DE HARDWARE EXTRAÍDA!" << RESET << endl;
                cout << CIANO << "   ➔ Endereço IP:     " << AMARELO << ip_invasor << RESET << endl;
                cout << CIANO << "   ➔ Sistema Operac.: " << BRANCO << os_detectado << RESET << endl;
                cout << CIANO << "   ➔ Vetor do Alvo:   " << AMARELO << tipo_requisicao << RESET << endl;
                cout << CIANO << "   ➔ Nível de Risco:  " << severidade << endl;
                cout << ROXO << "---------------------------------------------------------------------------------" << RESET << endl;

                // Resposta padrão HTTP Chunked para saturar e prender a ferramenta do atacante
                string resposta_http = 
                    "HTTP/1.1 200 OK\r\n"
                    "Server: " + NOME_SERVIDOR_FAKE + "\r\n"
                    "Content-Type: text/plain; charset=UTF-8\r\n"
                    "Transfer-Encoding: chunked\r\n"
                    "Connection: keep-alive\r\n\r\n";
                
                send(socket_alvo, resposta_http.c_str(), resposta_http.length(), 0);

                stringstream stream_hex;
                stream_hex << hex << VENENO_TERMINAL.length();
                string bloco_chunk = stream_hex.str() + "\r\n" + VENENO_TERMINAL + "\r\n";

                for (int i = 0; i < LINHAS_DE_INUNDACAO; ++i) {
                    if (send(socket_alvo, bloco_chunk.c_str(), bloco_chunk.length(), 0) < 0) {
                        break; 
                    }
                    usleep(10); 
                }

                send(socket_alvo, "0\r\n\r\n", 5, 0);
                close(socket_alvo);
                exit(0); 
            }
            close(socket_alvo); 
        }
    }
    return 0;
}
