//
// Created by lulos on 6/19/2025.
//

//todo cambiar nombre a analizador semantico. y sacar afuera de domain specific. 

#ifndef MICROGCC_ASTNAVIGATOR_H
#define MICROGCC_ASTNAVIGATOR_H

#include "SymbolTable.h"

int navigateAst(Program * program, SymbolTable * table);


#endif //MICROGCC_ASTNAVIGATOR_H

