#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "pung.hpp"

enum component_flags
{
  component_none = 0,
  component_rigidbody = (1 << 0),
  component_transform = (1 << 1),
  component_colour= (1 << 2),
  component_sprite = (1 << 3),
  component_playercontrol = (1 << 4),
  component_nncontrol = (1 << 5),
  component_simpleai = (1 << 6),
  component_agent = (1 << 7),
  component_text = (1 << 8),
};

typedef struct
{
} PlayerControl;

typedef struct
{
  sf::Text text;
  uint32 agent;
} Text;

typedef struct
{
  int32 score = 0;
  real32 target_y;
} Agent;

#include <SFML/Graphics.hpp>
typedef struct
{
  sf::Vector2f position;
  sf::Vector2f dimensions;
} Transform;


enum rb_shape
{
  rigidbody_circle,
  rigidbody_rectangle,
};

enum rb_type
{
  rigidbody_static,
  rigidbody_dynamic,
  rigidbody_kinematic,
};

#include <Box2D/Box2D.h>
typedef struct
{
  b2Body *rigidbody;
  rb_shape rigidbody_shape;
  rb_type rigidbody_type;
  real32 radius;
  real32 speed;
  real32 restitution;
} Rigidbody;

typedef struct
{
  sf::Sprite sprite;
} Sprite;


enum entity_type
{
  entity_null,
  entity_paddle,
  entity_ball,
  entity_score,
  entity_static,
};

typedef struct
{
  entity_type type;
} Type;

#endif
