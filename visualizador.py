import socket
import struct
import cv2
import numpy as np

PORTA_LOCAL = 8081

def iniciar_central_video():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(('0.0.0.0', PORTA_LOCAL))
    server.listen(1)
    print(f'\033[32m\033[1m[🟢 TRANSMISSÃO ATIVA] ➔ Local: {PORTA_LOCAL}\033[0m')

    conn, addr = server.accept()
    try:
        banner = conn.recv(1024).decode(errors="ignore")
        print(banner)
        conn.send(b"screen\n")
        cv2.namedWindow('Zodiac Stream - Janela Virtual (Bore)', cv2.WINDOW_NORMAL)
        while True:
            tamanho_bytes = conn.recv(4)
            if not tamanho_bytes or len(tamanho_bytes) < 4: 
                break
            
            # CORREÇÃO: Extrai o índice [0] para obter o valor inteiro do tamanho do buffer
            tamanho_total = struct.unpack("I", tamanho_bytes)[0]
            
            dados_imagem = b""
            while len(dados_imagem) < tamanho_total:
                bloco = conn.recv(tamanho_total - len(dados_imagem))
                if not bloco: 
                    break
                dados_imagem += bloco
                
            if len(dados_imagem) < tamanho_total: 
                break
                
            ajuste_np = np.frombuffer(dados_imagem, dtype=np.uint8)
            frame_visual = cv2.imdecode(ajuste_np, cv2.IMREAD_COLOR)
            if frame_visual is not None:
                cv2.imshow('Zodiac Stream - Janela Virtual (Bore)', frame_visual)
            if cv2.waitKey(1) & 0xFF == ord('q'): 
                break
    except Exception as e: 
        print(f"[-] Erro operacional: {e}")
    finally:
        cv2.destroyAllWindows()
        conn.close()
        server.close()

if __name__ == '__main__':
    iniciar_central_video()
