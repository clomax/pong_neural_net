#ifndef PUNG_HPP
#define PUNG_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cmath>
#include <random>
#include <algorithm>
#include <memory>
#include <cmath>
#include <stdlib.h>
#include <stdint.h>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <tclap/CmdLine.h>

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32   bool32;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float   real32;
typedef double  real64;

#if SLOW
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#else
#define Assert(Expression)
#endif

#endif
