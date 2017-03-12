
#ifndef STRUCTS_OPT_H
#define STRUCTS_OPT_H

#include "define.h"
#include "structs_container.h"

enum OptionFlag {
    OPT_TextureRemoveAll,
    OPT_WeaponRemoveAll,
    OPT_Help,
    OPT_Quiet,
    OPT_COUNT
};

typedef struct Opts {
    Buffer**    args;
    uint32_t    argCount;
    uint32_t    flags;
} Opts;

typedef struct OptHandler {
    const char* name;
    uint32_t    bit;
} OptHandler;

#endif/*STRUCTS_OPT_H*/
