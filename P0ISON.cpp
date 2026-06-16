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
vector<int> sockets_clientes; // Guarda a lista de conexões ativas para o Broadcast

void fechar_fortaleza(int sinal) {
    cout << "\n\n" << VERMELHO << "[-] [SISTEMA] Fechando conexões e liberando portas..." << RESET << endl;
    for (int fd : sockets_clientes) {
        close(fd);
    }
    close(socket_principal);
    exit(sinal);
}

int main() {
    signal(SIGINT, fechar_fortaleza);
    signal(SIGPIPE, SIG_IGN); // Bloqueia quedas por desconexão do alvo
    
    system("clear");

    int PORTA_ARMADILHA = 8081;          

    cout << ROXO << "=================================================================================" << RESET << endl;
    cout << CIANO << " ______  ____  _____   _    ____ " << endl;
    cout << CIANO << "|_  _  |/ __ \\|  __ \\ | |  / ___|" << endl;
    cout << CIANO << "  / /  | /  \\ | |  \\ \\| | | |      " << VERMELHO << "   [ OPERADOR SUPREMO ]" << endl;
    cout << CIANO << " / /__ | \\__/ | |__/ /| | | |___   " << BRANCO << "       ⚡ " << VERMELHO << PISCANDO << "N U L L" << RESET << BRANCO << " ⚡" << endl;
    cout << CIANO << "|_____| \\____/|_____/ |_|  \\____|  " << VERDE << "   (🔥 INTERACTIVE BROADCAST 🔥)" << endl;
    cout << ROXO << "=================================================================================" << RESET << endl;
    cout << VERDE << "   [+] Central Interativa ativa e monitorando a porta: " << BRANCO << PORTA_ARMADILHA << RESET << endl;
    cout << CIANO << "   [*] Digite mensagens no console para transmitir dados aos alvos conectados." << RESET << endl;
    cout << ROXO << "=================================================================================" << RESET << endl;
    cout << AMARELO << "   [*] Aguardando conexões... Digite sua transmissão a qualquer momento:\n" << RESET << endl;

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

    listen(socket_principal, 50);

    fd_set conjunto_leitura;

    while (true) {
        FD_ZERO(&conjunto_leitura);
        FD_SET(socket_principal, &conjunto_leitura);
        FD_SET(STDIN_FILENO, &conjunto_leitura); // Monitora o teclado do seu terminal

        int max_fd = socket_principal;
        for (int fd : sockets_clientes) {
            FD_SET(fd, &conjunto_leitura);
            if (fd > max_fd) max_fd = fd;
        }

        // Seleciona qual canal enviou dados (Teclado ou Rede) sem travar o processador
        int atividade = select(max_fd + 1, &conjunto_leitura, NULL, NULL, NULL);

        if (atividade < 0) continue;

        // 1. SE ENTRAR UMA NOVA CONEXÃO DE ATACANTE
        if (FD_SETVAL(socket_principal, &conjunto_leitura)) {
            struct sockaddr_in cliente;
            socklen_t tamanho_addr = sizeof(cliente);
            int novo_socket = accept(socket_principal, (struct sockaddr*)&cliente, &tamanho_addr);

            if (novo_socket >= 0) {
                string ip_invasor = inet_ntoa(cliente.sin_addr);
                sockets_clientes.push_back(novo_socket);

                cout << VERDE << "\n[🛰️ ALVO CONECTADO] ➔ Chassi de rede preso no terminal: " << AMARELO << ip_invasor 
                     << BRANCO << " (ID: " << novo_socket << ")" << RESET << endl;
                cout << CIANO << "ZODIAC_NULL_CONSOLE_> " << flush;
                
                // Envia uma mensagem de boas-vindas padrão HTTP para estabilizar a linha dele
                string welcome = "HTTP/1.1 200 OK\r\nContent-Type: text/plain; charset=UTF-8\r\n\r\n[CONEXÃO ESTABELECIDA COM O MAESTRO ZODIACO]\n";
                send(novo_socket, welcome.c_str(), welcome.length(), 0);
            }
        }

        // 2. SE VOCÊ DIGITAR ALGO NO SEU TECLADO
        if (FD_SETVAL(STDIN_FILENO, &conjunto_leitura)) {
            string entrada_usuario;
            getline(cin, entrada_usuario);

            if (!entrada_usuario.empty() && !sockets_clientes.empty()) {
                // Se você digitar "all: mensagem", envia para todos.
                // Se digitar "ID: mensagem" (ex: "4: tchau"), envia só para aquele alvo específico.
                size_t pos_dois_pontos = entrada_usuario.find(":");
                
                if (pos_dois_pontos != string::npos) {
                    string alvo = entrada_usuario.substr(0, pos_dois_pontos);
                    string msg_real = entrada_usuario.substr(pos_dois_pontos + 1) + "\n";

                    if (alvo == "all") {
                        // Envia para todo mundo ao mesmo tempo (Broadcast)
                        for (auto it = sockets_clientes.begin(); it != sockets_clientes.end(); ) {
                            if (send(*it, msg_real.c_str(), msg_real.length(), 0) < 0) {
                                close(*it);
                                it = sockets_clientes.erase(it);
                            } else {
                                ++it;
                            }
                        }
                        cout << VERDE << "[+] Transmissão enviada para todos os chassis ativos." << RESET << endl;
                    } else {
                        // Envia apenas para o ID selecionado
                        try {
                            int id_alvo = stoi(alvo);
                            send(id_alvo, msg_real.c_str(), msg_real.length(), 0);
                            cout << CIANO << "[+] Transmissão direcionada enviada ao ID: " << id_alvo << RESET << endl;
                        } catch (...) {
                            cout << VERMELHO << "[!] Formato inválido. Use 'all:texto' ou 'ID:texto' (Ex: 4:mensagem)" << RESET << endl;
                        }
                    }
                } else {
                    // Comportamento padrão: Se não colocar o prefixo, envia para todos
                    string msg_padrao = entrada_usuario + "\n";
                    for (auto it = sockets_clientes.begin(); it != sockets_clientes.end(); ) {
                        if (send(*it, msg_padrao.c_str(), msg_padrao.length(), 0) < 0) {
                            close(*it);
                            it = sockets_clientes.erase(it);
                        } else {
                            ++it;
                        }
                    }
                }
            }
            cout << CIANO << "ZODIAC_NULL_CONSOLE_> " << flush;
        }

        // 3. SE ALGUM ALVO SE DESCONECTAR OU ENVIAR DADOS
        for (auto it = sockets_clientes.begin(); it != sockets_clientes.end(); ) {
            if (FD_SETVAL(*it, &conjunto_leitura)) {
                char buffer = {0};
                int bytes_recebidos = recv(*it, buffer, sizeof(buffer) - 1, 0);

                if (bytes_recebidos <= 0) {
                    // Alvo fechou o terminal dele
                    cout << VERMELHO << "\n[-] [DESCONEXÃO] O alvo do ID " << *it << " fechou a conexão." << RESET << endl;
                    close(*it);
                    it = sockets_clientes.erase(it);
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
