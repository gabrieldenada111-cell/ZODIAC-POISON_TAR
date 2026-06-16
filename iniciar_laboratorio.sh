#!/bin/bash
clear

echo -e "\033[36m\033[1m[⚡] [ZODIAC SYSTEM] Iniciando rotinas de limpeza e auditoria...\033[0m"

# 1. Derruba processos antigos travados nas portas para evitar o "Address already in use"
sudo fuser -k 8081/tcp 2>/dev/null
sudo fuser -k 80/tcp 2>/dev/null
sudo killall -9 PoisonExecutable python3 2>/dev/null

echo -e "\033[32m\033[1m[🟢] Porta 8081 (Streaming) e Porta 80 (Câmera Web) LIBERADAS na RAM.\033[0m"
echo ""
echo -e "\033[33m\033[1m[!] INSTRUÇÕES OPERACIONAIS REVISADAS:\033[0m"
echo -e " ➔ Aba Atual: Central de Telemetria e Injeção de Rota Mestre."
echo -e " ➔ Para o Vídeo: Abra um terminal separado e digite: \033[36mpython3 visualizador.py\033[0m"
echo -e " ➔ Para o Túnel: Abra um terminal separado e digite: \033[36mbore local 8081 --to bore.pub\033[0m"
echo "========================================================================="
echo ""

# 2. Executa a Central Suprema Matrix em C++
./PoisonExecutable
