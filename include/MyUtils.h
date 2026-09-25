/*
 * Projeto: CONTROLE LEITOR IR LG
 * Versao: 1
 * Modificado: 2026-09-24 17:47:13
 */
#ifndef MY_UTILS_H
#define MY_UTILS_H

#include <Arduino.h>
#include <HWCDC.h>

const bool DEBUG_ACTIVE = true;

namespace MyUtils {
    inline void log(const String& msg) {
        if (DEBUG_ACTIVE) {
            Serial.println(msg);
        }
    }
}

#endif
