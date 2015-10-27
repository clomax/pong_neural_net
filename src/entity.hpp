#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "components.hpp"
#include "util.hpp"

#define ENTITY_COUNT 100


typedef struct
{
  int mask[ENTITY_COUNT];

  Agent         agent       [ENTITY_COUNT];
  Transform     transform   [ENTITY_COUNT];
  Rigidbody     rigidbody   [ENTITY_COUNT];
  Sprite        sprite      [ENTITY_COUNT];
  Type          type        [ENTITY_COUNT];
  Text          text        [ENTITY_COUNT];
  PlayerControl player      [ENTITY_COUNT];
} Mem;

inline Mem
createWorld()
{
  Mem world;
  for(int i=0; i<ENTITY_COUNT; ++i)
    world.mask[i] = component_none;
  return world;
}

inline std::vector<sf::Vector2f> add_transform(Mem *world, unsigned int entity,
    sf::Vector2f pos, sf::Vector2f dim);

unsigned int
createEntity(Mem *world, int flags = component_none)
{
  unsigned int entity;
  for(entity = 0; entity < ENTITY_COUNT; ++entity)
  {
    if(world->mask[entity] == component_none)
    {
      world->mask[entity] |= flags;
      world->type[entity].type = entity_null;
      return(entity);
    }
  }
  return(ENTITY_COUNT);
}

void
destroyEntity(Mem *world, unsigned int entity)
{
  world->mask[entity] = component_none;
}

inline void
set_flags(Mem *world, unsigned int entity, int flags)
{
  world->mask[entity] |= flags;
}

inline void
unset_flags(Mem *world, unsigned int entity, int flags)
{
  world->mask[entity] &= ~flags;
}

inline int
flag_is_set(Mem *world, unsigned int entity, int flag)
{
  return(world->mask[entity] & flag);
}

inline void
add_text(Mem *world, unsigned int entity, unsigned int agent, sf::Font *font, unsigned int size, sf::Vector2f pos)
{
  if (!flag_is_set(world, entity, component_text))
  {
    world->mask[entity] |= component_text;
  }

  world->text[entity].agent = agent;

  world->text[entity].text.setFont(*font);
  world->text[entity].text.setString(std::to_string(world->agent[agent].score));
  world->text[entity].text.setCharacterSize(size);
  world->text[entity].text.setOrigin(world->text[entity].text.getScale()/2.f);
  world->text[entity].text.setPosition(pos);
}

inline void
add_agent(Mem *world, unsigned int entity)
{
  if (!flag_is_set(world, entity, component_agent))
  {
    world->mask[entity] |= component_agent;
  }
}

inline b2Body*
add_rigidbody(Mem *world, unsigned int entity, b2World *phys_world, rb_type rt, rb_shape rs,
    sf::Vector2f pos = sf::Vector2f(0,0), sf::Vector2f dim = sf::Vector2f(10,10))
{
  if (!flag_is_set(world, entity, component_transform))
  {
    world->mask[entity] |= component_transform;
    add_transform(world, entity, pos, dim);
  }

  if (!flag_is_set(world, entity, component_rigidbody))
  {
    world->mask[entity] |= component_rigidbody;
  }

  b2BodyDef body_def;
  body_def.position = b2Vec2(
    world->transform[entity].position.x/SCALE,
    world->transform[entity].position.y/SCALE);

  body_def.type = b2_dynamicBody;
  switch(rt)
  {
    case rigidbody_static:
      body_def.type = b2_staticBody;
      break;
    case rigidbody_dynamic:
      body_def.type = b2_dynamicBody;
      break;
    case rigidbody_kinematic:
      body_def.type = b2_kinematicBody;
      break;
  }

  b2Body* body = phys_world->CreateBody(&body_def);
  world->rigidbody[entity].rigidbody = body;

  b2FixtureDef b2_fixture_def;
  b2_fixture_def.friction = 0.f;
  b2CircleShape c;
  b2PolygonShape p;

  switch(rs)
  {
    case rigidbody_circle:
      c.m_p = b2Vec2(0,0);
      c.m_radius = world->rigidbody[entity].radius;
      b2_fixture_def.shape = &c;
      b2_fixture_def.restitution = 1.f;
      world->rigidbody[entity].rigidbody->CreateFixture(&(b2_fixture_def));
      break;

    case rigidbody_rectangle:
      p.SetAsBox(
        (world->transform[entity].dimensions.x/2.f)/SCALE,
        (world->transform[entity].dimensions.y/2.f)/SCALE);
      b2_fixture_def.shape = &p;
      world->rigidbody[entity].rigidbody->CreateFixture(&b2_fixture_def);
      break;
  }

  return (world->rigidbody[entity].rigidbody);
}

inline std::vector<sf::Vector2f>
add_transform(Mem *world, unsigned int entity,
    sf::Vector2f pos = sf::Vector2f(0,0), sf::Vector2f dim = sf::Vector2f(10,10))
{
  if (!flag_is_set(world, entity, component_transform))
  {
    world->mask[entity] |= component_transform;
  }
  world->transform[entity].position = pos;
  world->transform[entity].dimensions = dim;
  return(std::vector<sf::Vector2f>{pos, dim});
}

inline sf::Sprite
add_sprite(Mem *world, unsigned int entity, sf::Texture *texture,
    sf::Vector2f pos = sf::Vector2f(0,0), sf::Vector2f dim = sf::Vector2f(10,10))
{
  if (!flag_is_set(world, entity, component_transform))
  {
    world->mask[entity] |= component_transform;
    add_transform(world, entity, pos, dim);
  }

  if (!flag_is_set(world, entity, component_sprite|component_sprite))
  {
    world->mask[entity] |= component_sprite;
  }

  world->transform[entity].position = pos;
  world->transform[entity].dimensions = dim;

  sf::Sprite s;
  Transform t = world->transform[entity];
  s.setTexture(*texture);
  s.setTextureRect(sf::IntRect(0,0,t.dimensions.x,t.dimensions.y));
  s.setPosition(t.position);
  s.setOrigin(sf::Vector2f(t.dimensions.x/2, t.dimensions.y/2));
  world->sprite[entity].sprite = s;

  return (world->sprite[entity].sprite);
}

#endif

