//
// Created by lulos on 6/21/2025.
//

#ifndef MICROGCC_GENERATORUTILS_H
#define MICROGCC_GENERATORUTILS_H


#include <stdio.h>
#include <stdarg.h>

void out(unsigned indentLevel, const char *fmt, ...);
void shutdownGeneratorUtils(void);

#endif
