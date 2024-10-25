#ifndef FUNCS_H
#define FUNCS_H

#include <angelscript.h>
#include "G:/steve/angelscript_2.37.0/add_on/scriptarray/scriptarray.h"
#include "G:/steve/angelscript_2.37.0/add_on/scriptstdstring/scriptstdstring.h"
#include <iostream>
#include "types.h"

// Typedefs
typedef uint8_t  byte;
typedef int8_t   int8;
typedef uint8_t  uint8;

typedef int16_t  int16;
typedef uint16_t uint16;

typedef uint32_t uint;
typedef float    float32;

typedef int64_t  int64;
typedef uint64_t uint64;
typedef double   double64;

void RegisterFunctions(asIScriptEngine* engine);

#endif // FUNCS_H
