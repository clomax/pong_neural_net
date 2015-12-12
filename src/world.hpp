#ifndef WORLD_HPP
#define WORLD_HPP

#define ENTITY_COUNT 100

#include "entity.hpp"

inline Mem *
createWorld()
{
  Mem *world = (Mem*)malloc(sizeof(Mem));
  for(int i=0; i<ENTITY_COUNT; ++i)
  {
    world->mask[i] = component_none;
  }
  return world;
}

#endif
