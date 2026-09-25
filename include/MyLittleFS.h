/*
 * Projeto: CONTROLE LEITOR IR LG
 * Versao: 2
 * Modificado: 2026-09-24 18:45:00
 */
#ifndef MY_LITTLEFS_H
#define MY_LITTLEFS_H

#include <Arduino.h>
#include <LittleFS.h>
#include "MyUtils.h"

namespace MyLittleFS {
    // Inicializa o LittleFS. Se falhar, tenta formatar se 'formatOnFail' for true.
    inline bool beginFS(bool formatOnFail = true) {
        if (!LittleFS.begin(formatOnFail)) {
            MyUtils::log("Erro crítico ao inicializar LittleFS!");
            return false;
        }
        MyUtils::log("LittleFS inicializado com sucesso.");
        return true;
    }

    // Grava uma string dentro de um arquivo especificado
    inline bool salvarArquivo(const char* path, const String& conteudo) {
        File file = LittleFS.open(path, "w");
        if (!file) {
            MyUtils::log("Falha ao abrir arquivo para escrita: " + String(path));
            return false;
        }
        file.print(conteudo);
        file.close();
        MyUtils::log("Arquivo salvo com sucesso: " + String(path));
        return true;
    }

    inline bool removerArquivo(const char* path) {
        if (!LittleFS.exists(path)) {
            return true;
        }
        return LittleFS.remove(path);
    }

    // Lê o conteúdo de um arquivo e retorna como String
    inline String lerArquivo(const char* path) {
        if (!LittleFS.exists(path)) {
            MyUtils::log("Arquivo nao encontrado: " + String(path));
            return "";
        }
        File file = LittleFS.open(path, "r");
        if (!file) {
            MyUtils::log("Falha ao abrir arquivo para leitura: " + String(path));
            return "";
        }
        String conteudo = file.readString();
        file.close();
        return conteudo;
    }
}

#endif
