/*
 * Projeto: CONTROLE LEITOR IR LG
 * Versao: 9
 * Modificado: 2026-09-24
 */
#include <Arduino.h>     // <-- DEVE SER SEMPRE A PRIMEIRA LINHA
#include <WiFi.h>
#include "MyUtils.h"
#include "MyLittleFS.h"
#include "MyWebWifi.h"

bool emModoConfiguracao = false;


void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println();
    Serial.println("=====================================");
    Serial.println("CONTROLE LEITOR IR LG - Versao 10");
    Serial.println("--- Inicializando Sistema Modular ---");
    Serial.println("=====================================");

    String espId = String((uint32_t)(ESP.getEfuseMac() & 0xFFFFFF), HEX);
    espId.toUpperCase();
    Serial.println("ESP ID: " + espId);

    // 1. Inicializa o Sistema de Arquivos
    if (!MyLittleFS::beginFS()) {
        return; 
    }

    // O app MIT sempre usa o AP fixo do ESP.
    String nomePontoAcesso = "PROJETO_APEX";
    Serial.println("SSID do ponto de acesso: " + nomePontoAcesso);
    MyWebWifi::iniciarPontoAcesso(nomePontoAcesso);

    // A conexão salva acontece em paralelo, sem desligar o AP do app.
    if (MyWebWifi::tentarConexaoSalva()) {
        Serial.println("Modo Operacional Ativo (AP + Wi-Fi local).");
        emModoConfiguracao = false;
    } else {
        Serial.println("Entrando em Modo de Configuracao...");
        emModoConfiguracao = true;
    }
}

void loop() {
    //Serial.println("Executando loop...");
    MyWebWifi::processarServidor();
}
