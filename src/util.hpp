#ifndef UTIL_HPP
#define UTIL_HPP

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 550
#define SCALE 30.f
#define PI 3.1459
#define E 2.71828182845904523536
#define DELIM ","

inline float
random_float (float min, float max)
{
  float random = ((float) rand()) / (float) RAND_MAX;
  float range = max - min;
  return (random*range) + min;
}

#endif
