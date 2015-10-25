
#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "util.hpp"

enum entity_type
{
  entity_null,
  entity_paddle,
  entity_ball,
  entity_score,
  entity_static,
};

enum rb_shape
{
  rigidbody_circle,
  rigidbody_rectangle,
};

enum rb_type
{
  rigidbody_static,
  rigidbody_dynamic,
};

enum component_flags
{
  component_rigidbody = (1 << 0),
  component_position = (1 << 1),
  component_dimensions = (1 << 2),
  component_colour= (1 << 3),
  component_sprite = (1 << 4),
  component_playercontrol = (1 << 5),
};

struct entity
{
  int component_flags;
  entity_type type;

  sf::Sprite *sprite;
  b2Body *rigidbody;
  rb_shape rigidbody_shape;
  rb_type rigidbody_type;

  sf::Vector2f position;
  sf::Vector2f dimensions;
  sf::Color colour;
  float radius;
  float restitution = 0.f;
};

inline int
flag_is_set(entity* e, int flag)
{
  return(e->component_flags & flag);
}

inline void
update_entity (entity* e)
{
  e->sprite->setPosition(
    SCALE * e->rigidbody->GetPosition().x,
    SCALE * e->rigidbody->GetPosition().y);
}

inline void
build_sprite(entity* e, sf::Sprite* s, sf::Texture* texture)
{
  e->sprite = s;
  e->sprite->setPosition(e->position);
  e->sprite->setTexture(*texture);
  e->sprite->setTextureRect(sf::IntRect(0,0,e->dimensions.x,e->dimensions.y));
  e->sprite->setOrigin(sf::Vector2f(e->dimensions.x/2, e->dimensions.y/2));
}

inline void
build_rigidbody(entity* e, b2World* world, rb_type rt, rb_shape rs)
{
  b2BodyDef body_def;
  body_def.position = b2Vec2(e->position.x/SCALE,e->position.y/SCALE);
  switch(rt)
  {
    case rigidbody_static:
      body_def.type = b2_staticBody;
      break;
    case rigidbody_dynamic:
      body_def.type = b2_dynamicBody;
      break;
    default:
      std::cerr << "Unknown rigidbody type.\n";
      exit(EXIT_FAILURE);
  }

  /* CreateBody and CreateFixture perform a deep copy on the
   * things you pass to them even though you're passing pointers.
   * _STOP FORGETTING THIS_
   */

  b2Body* body = world->CreateBody(&body_def);
  e->rigidbody = body;

  b2FixtureDef b2_fixture_def;
  b2CircleShape c;
  b2PolygonShape p;

  switch(rs)
  {
    case rigidbody_circle:
      c.m_p = b2Vec2(0,0);
      c.m_radius = e->radius;
      b2_fixture_def.shape = &c;
      b2_fixture_def.restitution = e->restitution;

      e->rigidbody->CreateFixture(&(b2_fixture_def));
      break;

    case rigidbody_rectangle:
      p.SetAsBox((e->dimensions.x/2.f)/SCALE, (e->dimensions.y/2.f)/SCALE);
      b2_fixture_def.shape = &p;
      e->rigidbody->CreateFixture(&b2_fixture_def);
      break;

    default:
      std::cerr << "Unknown shape.\n";
      exit(EXIT_FAILURE);
  }

  //break this out
  //b->rigidbody->SetLinearVelocity(b2Vec2(-e->speed, random_float(-3.f, 3.f)));
  //b2_fixture_def.density = 1.f;
  //b2_fixture_def.restitution = 1.f;
}
#endif
