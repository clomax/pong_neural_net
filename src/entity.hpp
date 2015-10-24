
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
  entity_divider,
};

enum component_flags
{
  component_rigidbody = (1 << 0),
  component_position = (1 << 1),
  component_dimensions = (1 << 2),
  component_colour= (1 << 3),
  component_sprite = (1 << 4),
};

struct entity
{
  int component_flags;
  entity_type type;

  sf::Sprite *sprite;
  b2Body *rigidbody;

  sf::Vector2f position;
  sf::Vector2f dimensions;
  sf::Color colour;
};

void
build_entity(entity* e, sf::Sprite* s, sf::Vector2f pos, sf::Vector2f dim, sf::Texture texture, sf::Color col)
{
  e->sprite = s;
  e->sprite->setPosition(pos);
  e->sprite->setColor(col);
  e->sprite->setTexture(texture);
  e->sprite->setTextureRect(sf::IntRect(0,0,dim.x,dim.y));
  e->sprite->setOrigin(sf::Vector2f(dim.x/2, dim.y/2));

/*
  b2BodyDef ball_body_def;
  ball_body_def.position = b2Vec2(10/SCALE,10/SCALE);
  ball_body_def.type = b2_dynamicBody;

  b2Body* ball_body = world->CreateBody(&ball_body_def);
  b->body = ball_body;

  ball_body->SetUserData(&b);

  b2CircleShape b2_ball_shape;
  b2_ball_shape.m_radius = b->radius/SCALE;

  b2FixtureDef b2_ball_fixture_def;
  b2_ball_fixture_def.shape = &b2_ball_shape;
  b2_ball_fixture_def.density = 1.f;
  b2_ball_fixture_def.restitution = 1.f;
  b->body->CreateFixture(&b2_ball_fixture_def);
  b->body->SetTransform(b2Vec2((SCREEN_WIDTH/2)/SCALE, (SCREEN_HEIGHT/2)/SCALE), 0);
  b->body->SetLinearVelocity(b2Vec2(-b->speed, random_float(-3.f, 3.f)));
*/
}

#endif
