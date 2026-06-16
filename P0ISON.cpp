#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <csignal>
#include <sstream>

using namespace std;

// Paleta de Cores Cyberpunk/Neon Avançada
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
    cout << "\n\n" << VERMELHO << "[-] [SISTEMA] Desconectando módulos e liberando portas do sistema..." << RESET << endl;
    close(socket_principal);
    exit(sinal);
}

int main() {
    signal(SIGINT, gangster_shutdown);
    system("clear");

    // ========================================================================
    // ⚙️ PAINEL DE CONFIGURAÇÃO INTERNA (MODIFIQUE LIVREMENTE AQUI)
    // ========================================================================
    int PORTA_ARMADILHA       = 8081;          
    int LINHAS_DE_INUNDACAO   = 100000;        
    string NOME_SERVIDOR_FAKE = "Nginx/1.24.0 (Ubuntu)"; 
    
    // Mensagem de saturação que traz o seu nick cravado no terminal do invasor
    string VENENO_TERMINAL    = "🚨 [ZODIAC BY NULL] -> OVERFLOW_CRITICAL_WARNING: SYSTEM_HIERARCHY_COMPROMISED_🚨\n";
    // ========================================================================

    // INTERFACE REESTRUTURADA: Logotipo Gigante ZODIAC com as barras corrigidas e alinhadas
    cout << ROXO << "=================================================================================" << RESET << endl;
    cout << CIANO << " ______  ____  _____   _    ____ " << endl;
    cout << CIANO << "|_  _  |/ __ \\|  __ \\ | |  / ___|" << endl;
    cout << CIANO << "  / /  | /  \\ | |  \\ \\| | | |      " << VERMELHO << "   [ OPERADOR SUPREMO ]" << endl;
    cout << CIANO << " / /__ | \\__/ | |__/ /| | | |___   " << BRANCO << "       ⚡ " << VERMELHO << PISCANDO << "N U L L" << RESET << BRANCO << " ⚡" << endl;
    cout << CIANO << "|_____| \\____/|_____/ |_|  \\____|  " << VERDE << "   (CRASH-ENGINE CONTRA SCANNERS)" << endl;
    cout << ROXO << "=================================================================================" << RESET << endl;
    cout << VERDE << "   [+] Sensores de Saturação linkados com sucesso na porta: " << BRANCO << PORTA_ARMADILHA << RESET << endl;
    cout << CIANO << "   [*] Armadilha ativa contra: Nmap, sqlmap, cURL, Python e Motores Web." << RESET << endl;
    cout << ROXO << "=================================================================================" << RESET << endl;
    cout << AMARELO << "   [*] Monitorando barramento de pacotes em tempo real...\n" << RESET << endl;

    socket_principal = socket(AF_INET, SOCK_STREAM, 0);
    int op = 1;
    setsockopt(socket_principal, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));

    struct sockaddr_in endereco;
    memset(&endereco, 0, sizeof(endereco));
    endereco.sin_family = AF_INET;
    endereco.sin_addr.s_addr = INADDR_ANY;
    endereco.sin_port = htons(PORTA_ARMADILHA);

    if (bind(socket_principal, (struct sockaddr*)&endereco, sizeof(endereco)) < 0) {
        cout << VERMELHO << "[!] ERRO CRÍTICO: A porta " << PORTA_ARMADILHA << " está bloqueada ou em uso." << RESET << endl;
        return 1;
    }

    listen(socket_principal, 20);

    // Loop atômico batizado com a sua assinatura de segurança customizada
    bool ZODIAC_NULL_OVERSIZE_PROTECTION = true;

    while (ZODIAC_NULL_OVERSIZE_PROTECTION) {
        struct sockaddr_in cliente;
        socklen_t tamanho_addr = sizeof(cliente);
        int socket_alvo = accept(socket_principal, (struct sockaddr*)&cliente, &tamanho_addr);

        if (socket_alvo >= 0) {
            string ip_invasor = inet_ntoa(cliente.sin_addr);
            
            char buffer_requisicao[2048] = {0};
            recv(socket_alvo, buffer_requisicao, sizeof(buffer_requisicao) - 1, 0);
            string dados_requisicao(buffer_requisicao);

            cout << VERMELHO << "🚨 [CAPTURA REALIZADA] ➔ CHASSI DE REDE INTERCEPTADO!" << RESET << endl;
            cout << CIANO << "   ➔ Endereço IP:   " << AMARELO << ip_invasor << RESET << endl;

            string ferramenta_detectada = "Conexão Manual/Desconhecida";
            if (dados_requisicao.find("nmap") != string::npos) {
                ferramenta_detectada = VERMELHO + PISCANDO + "NMAP NETWORK SCANNER 🔍" + RESET;
            } else if (dados_requisicao.find("sqlmap") != string::npos) {
                ferramenta_detectada = VERMELHO + PISCANDO + "SQLMAP INJECTION TOOL 🗄️" + RESET;
            } else if (dados_requisicao.find("python") != string::npos || dados_requisicao.find("curl") != string::npos) {
                ferramenta_detectada = AMARELO + "SCRIPT AUTOMATIZADO (Python/cURL) 🐍" + RESET;
            } else if (dados_requisicao.find("GET") != string::npos || dados_requisicao.find("POST") != string::npos) {
                ferramenta_detectada = VERDE + "Navegador Web Comercial 🌐" + RESET;
            }

            cout << CIANO << "   ➔ Assinatura:    " << ferramenta_detectada << endl;
            cout << CIANO << "   ➔ Contra-Medida: " << VERDE << "Inundando buffer com " << BRANCO << LINHAS_DE_INUNDACAO << VERDE << " rajadas de texto..." << RESET << endl;

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
                usleep(25); 
            }

            send(socket_alvo, "0\r\n\r\n", 5, 0);

            cout << VERDE << "[+] STATUS: Carga entregue. Conexão redefinida pela assinatura de NULL." << RESET << endl;
            cout << ROXO << "---------------------------------------------------------------------------------" << RESET << endl;
            close(socket_alvo);
        }
    }
    return 0;
}
