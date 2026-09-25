# esp-apex
Testes de conexão e troca de dados via API ou Download/Upload

## Arquitetura do sistema

### MIT App Inventor
- Aplicativo para conexão com o dispositivo ESP32.
- Gerenciamento da troca de dados usando **WebView**.
- Interface para consumo das páginas/rotas disponibilizadas pelo webserver embarcado.

### ESP32
- Execução de **Webserver** para disponibilizar interface e endpoints de comunicação.
- Suporte a **OTA (Over-The-Air)** para atualização remota de firmware.
- Responsável por receber, processar e responder às trocas de dados do app.
