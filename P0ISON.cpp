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
#include <iomanip>
#include <sstream>
#include <fstream>

using namespace std;

const string RESET     = "\033[0m";
const string ROXO      = "\033[35m\033[1m";
const string VERMELHO  = "\033[31m\033[1m";
const string VERDE     = "\033[32m\033[1m";
const string CIANO     = "\033[36m\033[1m";
const string AMARELO   = "\033[33m\033[1m";
const string BRANCO    = "\033[37m\033[1m";
const string PISCANDO  = "\033[5m";

struct Alvo {
    int id;
    string ip;
};

int socket_principal;
vector<Alvo> lista_alvos; 

void desenhar_menu_zodiac() {
    cout << ROXO << "=================================================================================" << RESET << endl;
    cout << CIANO << " ______  ____  _____   _    ____ " << endl;
    cout << CIANO << "|_  _  |/ __ \\|  __ \\ | |  / ___|" << endl;
    cout << CIANO << "  / /  | /  \\ | |  \\ \\| | | |      " << VERMELHO << "   [ OPERADOR SUPREMO ]" << endl;
    cout << CIANO << " / /__ | \\__/ | |__/ /| | | |___   " << BRANCO << "       ⚡ " << VERMELHO << PISCANDO << "N U L L" << RESET << BRANCO << " ⚡" << endl;
    cout << CIANO << "|_____| \\____/|_____/ |_|  \\____|  " << VERDE << "   (⚙️ MATRIX CORE SYSTEM v8.5 ⚙️)" << endl;
    cout << ROXO << "=================================================================================" << RESET << endl;
    cout << VERDE << "   [+] Comandos: 'list' (tabela) | 'clear' (limpar) | 'exit:all' (desligar tudo)" << RESET << endl;
    cout << CIANO << "   [*] Controle: 'kick:ID' (expulsar) | 'kick:ID:file' (expulsar + injetar txt)" << RESET << endl;
    cout << AMARELO << "   [*] Saturação: 'strike:ID' (destruir buffer) | 'strike:ID:file' (+ txt)" << RESET << endl;
    cout << ROXO << "=================================================================================" << RESET << endl;
    cout << AMARELO << "   [*] Central Zodíaco operacional. Aguardando conexões...\n" << RESET << endl;
}

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
    
    int PORTA_ARMADILHA  = 8081;          
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
    desenhar_menu_zodiac();

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

        // 1. RECEBIMENTO DE CONEXÕES DOS INTRUSOS
        if (FD_ISSET(socket_principal, &conjunto_leitura)) {
            struct sockaddr_in cliente;
            socklen_t tamanho_addr = sizeof(cliente);
            int novo_socket = accept(socket_principal, (struct sockaddr*)&cliente, &tamanho_addr);

            if (novo_socket >= 0) {
                Alvo novo_alvo;
                novo_alvo.id = novo_socket;
                novo_alvo.ip = inet_ntoa(cliente.sin_addr);
                lista_alvos.push_back(novo_alvo);

                cout << VERDE << "\n[🛰️ ALVO INTERCEPTADO] ➔ Chassi na linha de escuta: " 
                     << AMARELO << novo_alvo.ip << BRANCO << " (ID: " << novo_alvo.id << ")" << RESET << endl;
                cout << CIANO << "ZODIAC_NULL_CONSOLE_> " << flush;
                
                string welcome = "HTTP/1.1 200 OK\r\nContent-Type: text/plain; charset=UTF-8\r\n\r\n[CONEXÃO MONITORADA PELO NÚCLEO SUPREMO ZODÍACO]\n";
                send(novo_socket, welcome.c_str(), welcome.length(), 0);
            }
        }

        // 2. COMANDOS DO CONSOLE INTERATIVO DO TECLADO
        if (FD_ISSET(STDIN_FILENO, &conjunto_leitura)) {
            string entrada;
            getline(cin, entrada);

            if (entrada == "clear") {
                system("clear");
                desenhar_menu_zodiac();
            }
            else if (entrada == "exit:all") {
                cout << VERMELHO << "\n[-] [FECHAMENTO] Executando comando de ejeção global em lote..." << RESET << endl;
                for (const auto& alvo : lista_alvos) {
                    close(alvo.id);
                }
                close(socket_principal);
                cout << VERDE << "[+] Central desligada com sucesso." << RESET << endl;
                exit(0);
            }
            else if (entrada == "list") {
                cout << ROXO << "\n+-------------------------------------------------------+" << RESET << endl;
                cout << CIANO << "|             TABELA DE CHASSIS ATIVOS (NULL)           |" << RESET << endl;
                cout << ROXO << "+----------+-------------------+------------------------+" << RESET << endl;
                cout << AMARELO << "| INDEX ID | ENDEREÇO IP REAL  | STATUS DO MONITOR      |" << endl;
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
            else if (!entrada.empty()) {
                size_t pos_dois_pontos = entrada.find(":");
                if (pos_dois_pontos != string::npos) {
                    string prefixo = entrada.substr(0, pos_dois_pontos);
                    string resto = entrada.substr(pos_dois_pontos + 1);

                    if (prefixo == "kick" || prefixo == "strike") {
                        size_t sub_dois_pontos = resto.find(":");
                        string id_str = (sub_dois_pontos == string::npos) ? resto : resto.substr(0, sub_dois_pontos);
                        bool linkar_arquivo_local = (sub_dois_pontos != string::npos && resto.substr(sub_dois_pontos + 1) == "file");

                        try {
                            int id_alvo = stoi(id_str);
                            bool achou = false;

                            for (auto it = lista_alvos.begin(); it != lista_alvos.end(); ++it) {
                                if (it->id == id_alvo) {
                                    achou = true;
                                    
                                    if (linkar_arquivo_local) {
                                        ifstream arquivo_local("/home/gabriel/POISON/aviso.txt");
                                        string conteudo_arquivo = "";
                                        
                                        if (arquivo_local.is_open()) {
                                            string linha;
                                            while (getline(arquivo_local, linha)) {
                                                conteudo_arquivo += linha + "\n";
                                            }
                                            arquivo_local.close();
                                        } else {
                                            conteudo_arquivo = "AVISO SUPREMO ZODIACO: Conexao encerrada pelo operador NULL.\n";
                                        }

                                        string payload_transmissao = 
                                            "HTTP/1.1 200 OK\r\n"
                                            "Content-Type: text/plain\r\n"
                                            "Content-Disposition: attachment; filename=\"ZODIAC_WARNING.txt\"\r\n\r\n" + conteudo_arquivo;
                                        
                                        send(id_alvo, payload_transmissao.c_str(), payload_transmissao.length(), 0);
                                        cout << VERDE << "[+] Arquivo local 'aviso.txt' redirecionado com sucesso." << RESET << endl;
                                    }

                                    if (prefixo == "kick") {
                                        cout << VERMELHO << "\n[-] [EJEÇÃO] Expulsando o ID " << id_alvo << " do barramento." << RESET << endl;
                                        close(id_alvo);
                                    } else {
                                        cout << VERMELHO << "\n🔥 [ZODIAC STRIKE] -> Descarregando saturação no ID: " << id_alvo << RESET << endl;
                                        string carga = "🚨 [ZODIAC OVERSIZE CRASH BY NULL] 🚨\n";
                                        for (int i = 0; i < 100000; ++i) {
                                    if (prefixo == "kick") {
                                        cout << VERMELHO << "\n[-] [EJEÇÃO] Expulsando o ID " << id_alvo << " do barramento." << RESET << endl;
                                        close(id_alvo);
                                    } else {
                                        cout << VERMELHO << "\n🔥 [ZODIAC STRIKE] -> Descarregando saturação no ID: " << id_alvo << RESET << endl;
                                        string carga = "🚨 [ZODIAC OVERSIZE CRASH BY NULL] 🚨\n";
                                        for (int i = 0; i < 100000; ++i) {
                                            if (send(id_alvo, carga.c_str(), carga.length(), 0) < 0) break;
                                        }
                                        close(id_alvo);
                                    }

                                    lista_alvos.erase(it);
                                    break;
                                }
                            }
                            if (!achou) cout << VERMELHO << "[!] ID não encontrado." << RESET << endl;
                        } 
                        catch (...) {
                            cout << VERMELHO << "[!] Use 'kick:ID:file' ou 'strike:ID:file'" << RESET << endl;
                        }
                    }
                    else {
                        string msg_real = "🚨 [AVISO ZODIACO]: " + resto + "\n";
                        if (prefixo == "all") {
                            for (auto it = lista_alvos.begin(); it != lista_alvos.end(); ) {
                                if (send(it->id, msg_real.c_str(), msg_real.length(), 0) < 0) {
                                    close(it->id);
                                    it = lista_alvos.erase(it);
                                } else {
                                    ++it;
                                }
                            }
                        } else {
                            try {
                                int id_alvo = stoi(prefixo);
                                send(id_alvo, msg_real.c_str(), msg_real.length(), 0);
                            } catch (...) {}
                        }
                    }
                }
            }
            cout << CIANO << "ZODIAC_NULL_CONSOLE_> " << flush;
        }

        // 3. MONITORAMENTO DE REDE E QUEDAS DE CONEXÃO (CORRIGIDO)
        for (auto it = lista_alvos.begin(); it != lista_alvos.end(); ) {
            if (FD_ISSET(it->id, &conjunto_leitura)) {
                char buf[1024] = {0}; // Buffer alocado como array para aceitar ponteiro void* legítimo
                if (recv(it->id, buf, sizeof(buf) - 1, 0) <= 0) {
                    cout << VERMELHO << "\n[-] [DESCONEXÃO] O alvo do ID " << it->id << " fugiu." << RESET << endl;
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
