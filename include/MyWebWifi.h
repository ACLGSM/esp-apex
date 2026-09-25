/*
 * Projeto: CONTROLE LEITOR IR LG
 * Versao: 8
 * Modificado: 2026-09-24 18:13:00
 */
#ifndef MY_WEB_WIFI_H
#define MY_WEB_WIFI_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "MyUtils.h"
#include "MyLittleFS.h"

namespace MyWebWifi {
    // Instanciação interna do servidor na porta padrão 80
    static WebServer server(80);
    static DNSServer dnsServer;
    static bool tentativaEmAndamento = false;
    static bool cabecalhoPortalExibido = false;
    static bool servidorAtivo = false;
    static bool pontoAcessoAtivo = false;
    static bool wifiLocalConectado = false;

    inline void mostrarInformacoesConexao(const String& origem) {
        Serial.println();
        Serial.println("========== WI-FI CONECTADO ==========");
        Serial.println("Origem: " + origem);
        Serial.println("SSID: " + WiFi.SSID());
        Serial.println("IP do ESP32: " + WiFi.localIP().toString());
        Serial.println("Gateway: " + WiFi.gatewayIP().toString());
        Serial.println("Mascara: " + WiFi.subnetMask().toString());
        Serial.println("RSSI: " + String(WiFi.RSSI()) + " dBm");
        Serial.println("======================================");
    }

    inline String escaparHtml(const String& valor) {
        String resultado = valor;
        resultado.replace("&", "&amp;");
        resultado.replace("<", "&lt;");
        resultado.replace(">", "&gt;");
        resultado.replace("\"", "&quot;");
        resultado.replace("'", "&#39;");
        return resultado;
    }

    inline String escaparJson(const String& valor) {
        String resultado = valor;
        resultado.replace("\\", "\\\\");
        resultado.replace("\"", "\\\"");
        resultado.replace("\r", "\\r");
        resultado.replace("\n", "\\n");
        return resultado;
    }

    inline String obterOpcoesSsid(const String& selecionado = "") {
        String selecaoInicial = selecionado.length() == 0 ? " selected" : "";
        return "<option value='' disabled" + selecaoInicial + ">Carregando redes Wi-Fi...</option>";
    }

    inline String montarPaginaConfiguracao(const String& mensagem = "", const String& ssid = "") {
        String aviso = mensagem.length() > 0 ? "<p class='aviso'>" + escaparHtml(mensagem) + "</p>" : "";
        String html = "<!doctype html><html lang='pt-BR'><head>"
                      "<meta name='viewport' content='width=device-width,initial-scale=1'>"
                      "<meta http-equiv='Cache-Control' content='no-store'>"
                      "<meta charset='utf-8'><title>Configurar Wi-Fi</title>"
                      "<style>"
                      "*{box-sizing:border-box}body{margin:0;min-height:100vh;"
                      "font-family:Arial,sans-serif;background:#eef2f5;color:#18232d;"
                      "display:flex;align-items:center;justify-content:center;padding:24px}"
                      "main{width:min(100%,560px);background:#fff;border-radius:12px;"
                      "padding:clamp(24px,7vw,48px);box-shadow:0 8px 28px #0002}"
                      "h1{font-size:clamp(1.6rem,5vw,2.3rem);margin:0 0 28px}"
                      ".aviso{padding:14px;border-radius:8px;background:#fff3cd;color:#664d03;"
                      "font-weight:600}label{display:block;font-size:1.1rem;font-weight:600;margin:20px 0 8px}"
                      "select,input[type=password],input[type=text],button{width:100%;min-height:52px;"
                      "border:1px solid #aab7c2;border-radius:8px;padding:12px;font-size:1.05rem;background:#fff}"
                      ".mostrar{display:flex;align-items:center;gap:8px;font-size:1rem;font-weight:400}"
                      ".mostrar input{width:20px;height:20px}button{margin-top:28px;border:0;"
                      "background:#1769aa;color:#fff;font-weight:700;cursor:pointer}"
                      "button:active{background:#0f4f82}.status{min-height:24px;font-weight:600}"
                      "</style></head><body><main>"
                      "<h1>Configurar Wi-Fi</h1>" + aviso +
                      "<form action='/salvar' method='POST' autocomplete='off' onsubmit=\"document.getElementById('status').textContent='Testando conexao...';document.getElementById('enviar').disabled=true;\">"
                      "<label for='ssid'>Rede Wi-Fi</label>"
                      "<select id='ssid' name='ssid' required>" + obterOpcoesSsid(ssid) + "</select>"
                      "<label for='senha'>Senha</label>"
                      "<input id='senha' type='password' name='senha' autocomplete='current-password'>"
                      "<label class='mostrar'><input type='checkbox' onclick=\"document.getElementById('senha').type=this.checked?'text':'password'\">Mostrar senha</label>"
                      "<p id='status' class='status' aria-live='polite'></p>"
                      "<button id='enviar' type='submit'>Testar e conectar</button>"
                      "</form><script>"
                      "const lista=document.getElementById('ssid');"
                      "fetch('/api/redes').then(r=>r.json()).then(redes=>{"
                      "lista.innerHTML='<option value=\"\" disabled selected>Selecione uma rede Wi-Fi</option>';"
                      "redes.forEach(rede=>{const opcao=document.createElement('option');opcao.value=rede;opcao.textContent=rede;lista.appendChild(opcao);});"
                      "}).catch(()=>{lista.innerHTML='<option value=\"\" disabled selected>Falha ao buscar redes</option>';});"
                      "</script></main></body></html>";
        return html;
    }

    // Rota que exibe o formulário de configuração caso não esteja conectado
    inline void handleRootConfig() {
        if (!cabecalhoPortalExibido) {
            MyUtils::log("CONTROLE LEITOR IR LG - Versao 4");
            cabecalhoPortalExibido = true;
        }
        server.sendHeader("Cache-Control", "no-store");
        server.send(200, "text/html", montarPaginaConfiguracao());
    }

    inline void handlePortalRequest() {
        handleRootConfig();
    }

    inline void handleApiStatus() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        String resposta = "{\"wifi_local_conectado\":" + String(wifiLocalConectado ? "true" : "false") +
                          ",\"modo_configuracao\":" + String(!wifiLocalConectado ? "true" : "false") +
                          ",\"ip_ap\":\"" + escaparJson(WiFi.softAPIP().toString()) + "\"" +
                          ",\"ip_local\":\"" + escaparJson(WiFi.localIP().toString()) + "\"" +
                          ",\"url\":\"http://PROJETO_APEX/\"}";
        server.send(200, "application/json", resposta);
    }

    inline void handleApiRedes() {
        int quantidade = WiFi.scanNetworks();
        String resposta = "[";
        bool primeiraRede = true;

        for (int indice = 0; indice < quantidade; indice++) {
            String ssid = WiFi.SSID(indice);
            if (ssid.length() == 0) {
                continue;
            }

            if (!primeiraRede) {
                resposta += ",";
            }
            resposta += "\"" + escaparJson(ssid) + "\"";
            primeiraRede = false;
        }

        WiFi.scanDelete();
        resposta += "]";
        server.send(200, "application/json", resposta);
    }

    inline void handleSalvar();

    inline void iniciarServidor() {
        if (servidorAtivo) {
            return;
        }

        server.on("/", handleRootConfig);
        server.on("/salvar", HTTP_POST, handleSalvar);
        server.on("/api/status", HTTP_GET, handleApiStatus);
        server.on("/api/redes", HTTP_GET, handleApiRedes);
        server.onNotFound(handlePortalRequest);
        server.begin();
        servidorAtivo = true;
        MyUtils::log("Servidor web iniciado na porta 80.");
    }

    // Rota que recebe os dados enviados pelo formulário/MIT App
    inline void handleSalvar() {
        if (!server.hasArg("ssid") || !server.hasArg("senha")) {
            server.send(400, "text/html", montarPaginaConfiguracao("Preencha a rede e a senha."));
            return;
        }

        if (tentativaEmAndamento) {
            server.send(409, "text/html", montarPaginaConfiguracao("Ja existe uma tentativa em andamento. Aguarde o resultado."));
            return;
        }
        tentativaEmAndamento = true;

        String ssid = server.arg("ssid");
        String senha = server.arg("senha");
        ssid.trim();

        MyUtils::log("Inicio do teste de conexao Wi-Fi. SSID recebido: [" + ssid + "] senha com " + String(senha.length()) + " caracteres.");
        WiFi.disconnect(false, false);
        delay(100);
        WiFi.mode(WIFI_AP_STA);
        WiFi.begin(ssid.c_str(), senha.c_str());

        int tentativas = 0;
        while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
            delay(500);
            tentativas++;
        }

        if (WiFi.status() != WL_CONNECTED) {
            MyUtils::log("Falha na conexao Wi-Fi. Status: " + String(WiFi.status()));
            wifiLocalConectado = false;
            WiFi.disconnect(false, false);
            tentativaEmAndamento = false;
            MyLittleFS::removerArquivo("/wifi_config.txt");
            server.sendHeader("Cache-Control", "no-store");
            server.send(200, "text/html", montarPaginaConfiguracao("Nao foi possivel conectar. Confira a senha e tente novamente.", ssid));
            return;
        }

        mostrarInformacoesConexao("configuracao pelo portal/app");
        wifiLocalConectado = true;
        String dados = ssid + "\n" + senha;
        bool arquivoSalvo = MyLittleFS::salvarArquivo("/wifi_config.txt", dados);
        String dadosConfirmados = MyLittleFS::lerArquivo("/wifi_config.txt");
        if (!arquivoSalvo || dadosConfirmados != dados) {
            MyUtils::log("ERRO: credenciais nao foram confirmadas no LittleFS.");
            MyLittleFS::removerArquivo("/wifi_config.txt");
            tentativaEmAndamento = false;
            server.send(500, "text/html", montarPaginaConfiguracao("A conexao funcionou, mas nao foi possivel salvar as credenciais."));
            return;
        }

        MyUtils::log("Credenciais confirmadas no LittleFS.");
        server.send(200, "text/html", "<html><body><h2>Conexao realizada com sucesso.</h2><p>O ESP32 sera reiniciado.</p></body></html>");
        delay(2000);
        ESP.restart();
    }

    // Configura e inicia o Ponto de Acesso próprio do ESP32
    inline void iniciarPontoAcesso(const String& ap_ssid) {
        WiFi.mode(WIFI_AP_STA);
        WiFi.softAP(ap_ssid.c_str());
        pontoAcessoAtivo = true;
        dnsServer.start(53, "*", WiFi.softAPIP());
        
        MyUtils::log("Ponto de Acesso Criado!");
        MyUtils::log("Conecte-se na rede: " + ap_ssid);
        MyUtils::log("IP do ESP32: " + WiFi.softAPIP().toString());

        iniciarServidor();
    }

    // Escuta as requisições (deve ser chamado no loop principal)
    inline void processarServidor() {
        if (pontoAcessoAtivo) {
            dnsServer.processNextRequest();
        }
        if (servidorAtivo) {
            server.handleClient();
        }
    }

    // Tenta conectar ao Wi-Fi local usando os dados salvos no LittleFS
    inline bool tentarConexaoSalva() {
        String dados = MyLittleFS::lerArquivo("/wifi_config.txt");
        if (dados == "") {
            MyUtils::log("Nenhuma credencial de Wi-Fi salva.");
            return false;
        }

        // Separa o SSID e Senha vindos do arquivo
        int indexN = dados.indexOf('\n');
        if (indexN <= 0) {
            MyUtils::log("Arquivo de credenciais invalido.");
            MyLittleFS::removerArquivo("/wifi_config.txt");
            return false;
        }
        String ssid = dados.substring(0, indexN);
        String senha = dados.substring(indexN + 1);

        ssid.trim();
        senha.replace("\r", "");

        MyUtils::log("Credenciais salvas:");
        MyUtils::log("  SSID: " + ssid);
        MyUtils::log("Tentando conectar na rede salva: " + ssid);
        WiFi.mode(WIFI_AP_STA);
        WiFi.setAutoReconnect(true);
        WiFi.begin(ssid.c_str(), senha.c_str());

        // Aguarda até 15 segundos pela conexão salva.
        int tentativas = 0;
        while (WiFi.status() != WL_CONNECTED && tentativas < 30) {
            delay(500);
            Serial.print(".");
            tentativas++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            wifiLocalConectado = true;
            mostrarInformacoesConexao("credenciais salvas");
            return true;
        } else {
            wifiLocalConectado = false;
            MyUtils::log("\nFalha ao conectar na rede salva.");
            MyLittleFS::removerArquivo("/wifi_config.txt");
            MyUtils::log("Credenciais removidas. Entrando no modo de configuracao.");
            return false;
        }
    }
}

#endif
