#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <csignal>
#include <vector>
#include <sys/select.h>
#include <iomanip> // Necessário para alinhar a tabela na tela

using namespace std;

const string RESET     = "\033[0m";
const string ROXO      = "\033[35m\033[1m";
const string VERMELHO  = "\033[31m\033[1m";
const string VERDE     = "\033[32m\033[1m";
const string CIANO     = "\033[36m\033[1m";
const string AMARELO   = "\033[33m\033[1m";
const string BRANCO    = "\033[37m\033[1m";
const string PISCANDO  = "\033[5m";

// Estrutura para guardar os dados dos alvos na tabela
struct Alvo {
    int id;
    string ip;
};

int socket_principal;
vector<Alvo> lista_alvos; 

void fechar_fortaleza(int sinal) {
    cout << "\n\n" << VERMELHO << "[-] [SISTEMA] Fechando conexões e liberando portas..." << RESET << endl;
    for (const auto& alvo : lista_alvos) {
        close(alvo.id);
    }
    close(socket_principal);
    exit(sinal);
}

int main() {
    signal(SIGINT, fechar_fortaleza);
    signal(SIGPIPE, SIG_IGN); 
    
    system("clear");

    int PORTA_ARMADILHA  = 8081;          

    cout << ROXO << "=================================================================================" << RESET << endl;
    cout << CIANO << " ______  ____  _____   _    ____ " << endl;
    cout << CIANO << "|_  _  |/ __ \\|  __ \\ | |  / ___|" << endl;
    cout << CIANO << "  / /  | /  \\ | |  \\ \\| | | |      " << VERMELHO << "   [ OPERADOR SUPREMO ]" << endl;
    cout << CIANO << " / /__ | \\__/ | |__/ /| | | |___   " << BRANCO << "       ⚡ " << VERMELHO << PISCANDO << "N U L L" << RESET << BRANCO << " ⚡" << endl;
    cout << CIANO << "|_____| \\____/|_____/ |_|  \\____|  " << VERDE << "   (⚙️ MATRIX CONSOLE SYSTEM ⚙️)" << endl;
    cout << ROXO << "=================================================================================" << RESET << endl;
    cout << VERDE << "   [+] Central ativa. Digite 'list' para ver a tabela de chassis ativos." << RESET << endl;
    cout << CIANO << "   [*] Comandos: 'list' (tabela) | 'all:texto' (massa) | 'ID:texto' (direcionado)" << RESET << endl;
    cout << ROXO << "=================================================================================" << RESET << endl;
    cout << AMARELO << "   [*] Aguardando conexões...\n" << RESET << endl;

    socket_principal = socket(AF_INET, SOCK_STREAM, 0);
    int op = 1;
    setsockopt(socket_principal, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));

    struct sockaddr_in endereco;
    memset(&endereco, 0, sizeof(endereco));
    endereco.sin_family = AF_INET;
    endereco.sin_addr.s_addr = INADDR_ANY;
    endereco.sin_port = htons(PORTA_ARMADILHA);

    if (bind(socket_principal, (struct sockaddr*)&endereco, sizeof(endereco)) < 0) {
        cout << VERMELHO << "[!] ERRO CRÍTICO: A porta " << PORTA_ARMADILHA << " está ocupada." << RESET << endl;
        return 1;
    }

    listen(socket_principal, 50);

    fd_set conjunto_leitura;

    while (true) {
        FD_ZERO(&conjunto_leitura);
        FD_SET(socket_principal, &conjunto_leitura);
        FD_SET(STDIN_FILENO, &conjunto_leitura); 

        int max_fd = socket_principal;
        for (const auto& alvo : lista_alvos) {
            FD_SET(alvo.id, &conjunto_leitura);
            if (alvo.id > max_fd) max_fd = alvo.id;
        }

        int atividade = select(max_fd + 1, &conjunto_leitura, NULL, NULL, NULL);
        if (atividade < 0) continue;

        // 1. RECEBENDO UM NOVO CHASSI DE REDE
        if (FD_ISSET(socket_principal, &conjunto_leitura)) {
            struct sockaddr_in cliente;
            socklen_t tamanho_addr = sizeof(cliente);
            int novo_socket = accept(socket_principal, (struct sockaddr*)&cliente, &tamanho_addr);

            if (novo_socket >= 0) {
                Alvo novo_alvo;
                novo_alvo.id = novo_socket;
                novo_alvo.ip = inet_ntoa(cliente.sin_addr);
                lista_alvos.push_back(novo_alvo);

                cout << VERDE << "\n[🛰️ ALVO INTERCEPTADO] ➔ Novo chassi adicionado à tabela de controle: " 
                     << AMARELO << novo_alvo.ip << BRANCO << " (ID: " << novo_alvo.id << ")" << RESET << endl;
                cout << CIANO << "ZODIAC_NULL_CONSOLE_> " << flush;
                
                string welcome = "HTTP/1.1 200 OK\r\nContent-Type: text/plain; charset=UTF-8\r\n\r\n[CONEXÃO MONITORADA PELO NÚCLEO SUPREMO ZODÍACO]\n";
                send(novo_socket, welcome.c_str(), welcome.length(), 0);
            }
        }

        // 2. PROCESSAMENTO DO SEU TECLADO (COMANDO LIST E ALERTAS)
        if (FD_ISSET(STDIN_FILENO, &conjunto_leitura)) {
            string entrada;
            getline(cin, entrada);

            if (entrada == "list") {
                // DESENHA A TABELA CYBERPUNK NA TELA DO TERMINAL
                cout << ROXO << "\n+-------------------------------------------------------+" << RESET << endl;
                cout << CIANO << "|             TABELA DE CHASSIS ATIVOS (NULL)           |" << RESET << endl;
                cout << ROXO << "+----------+-------------------+------------------------+" << RESET << endl;
                cout << AMARELO << "| INDEX ID | ENDEREÇO IP REAL  | STATUS DO MONITOR      |" << RESET << endl;
                cout << ROXO << "+----------+-------------------+------------------------+" << RESET << endl;
                
                if (lista_alvos.empty()) {
                    cout << BRANCO << "|      [ NENHUM INTRUSO PRESO NA ARMADILHA ATUALMENTE ]        |" << RESET << endl;
                } else {
                    for (const auto& alvo : lista_alvos) {
                        cout << BRANCO << "| " << setw(8) << alvo.id 
                             << " | " << setw(17) << alvo.ip 
                             << " | " << VERDE << "LINHA MONITORADA 🟢 " << BRANCO << " |" << RESET << endl;
                    }
                }
                cout << ROXO << "+----------+-------------------+------------------------+\n" << RESET << endl;
            }
            else if (!entrada.empty() && !lista_alvos.empty()) {
                size_t pos_dois_pontos = entrada.find(":");
                if (pos_dois_pontos != string::npos) {
                    string prefixo = entrada.substr(0, pos_dois_pontos);
                    string msg_real = "🚨 [AVISO ZODIACO]: " + entrada.substr(pos_dois_pontos + 1) + "\n";

                    if (prefixo == "all") {
                        for (auto it = lista_alvos.begin(); it != lista_alvos.end(); ) {
                            if (send(it->id, msg_real.c_str(), msg_real.length(), 0) < 0) {
                                close(it->id);
                                it = lista_alvos.erase(it);
                            } else {
                                ++it;
                            }
                        }
                        cout << VERDE << "[+] Alerta em massa injetado nos terminais." << RESET << endl;
                    } else {
                        try {
                            int id_alvo = stoi(prefixo);
                            send(id_alvo, msg_real.c_str(), msg_real.length(), 0);
                            cout << CIANO << "[+] Alerta direcionado enviado ao ID: " << id_alvo << RESET << endl;
                        } catch (...) {
                            cout << VERMELHO << "[!] Use 'list' ou 'all:texto' ou 'ID:texto'." << RESET << endl;
                        }
                    }
                }
            }
            cout << CIANO << "ZODIAC_NULL_CONSOLE_> " << flush;
        }

        // 3. MONITORAMENTO DE DESCONEXÕES
        for (auto it = lista_alvos.begin(); it != lista_alvos.end(); ) {
            if (FD_ISSET(it->id, &conjunto_leitura)) {
                char buf = {0};
                if (recv(it->id, buf, sizeof(buf) - 1, 0) <= 0) {
                    cout << VERMELHO << "\n[-] [DESCONEXÃO] O alvo do ID " << it->id << " quebrou a linha e fugiu." << RESET << endl;
                    close(it->id);
                    it = lista_alvos.erase(it);
                    cout << CIANO << "ZODIAC_NULL_CONSOLE_> " << flush;
                } else {
                    ++it;
                }
            } else {
                ++it;
            }
        }
    }
    return 0;
}
