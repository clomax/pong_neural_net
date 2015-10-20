#ifndef PADDLE_HPP
#define PADDLE_HPP

#include <memory>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "util.hpp"

struct paddle
{
  sf::RectangleShape shape;
  sf::Vector2f pos;
  sf::Vector2f dim;
  sf::Color colour;
  b2Body *body;
  float target_y;
  int score = 0;
};

std::unique_ptr<paddle>
create_paddle(b2World* world, sf::Vector2f position, sf::Vector2f dimension, sf::Color colour)
{
  std::unique_ptr<paddle> p(new paddle);
  p->pos = position;
  p->dim = dimension;
  p->colour = colour;
  p->score = 0;

  b2BodyDef paddle_body_def;
  paddle_body_def.position = b2Vec2(position.x/SCALE, position.y/SCALE);
  paddle_body_def.type = b2_staticBody;
  b2Body* paddle_body = world->CreateBody(&paddle_body_def);
  p->body = paddle_body;

  b2PolygonShape b2_paddle_shape;
  b2_paddle_shape.SetAsBox((dimension.x/2)/SCALE, (dimension.y/2)/SCALE);

  b2FixtureDef b2_paddle_fixture_def;
  b2_paddle_fixture_def.shape = &b2_paddle_shape;
  b2_paddle_fixture_def.density = 1.f;
  p->body->CreateFixture(&b2_paddle_fixture_def);

  sf::RectangleShape paddle_shape;
  p->shape = paddle_shape;
  p->shape.setOrigin(sf::Vector2f(dimension.x/2, dimension.y/2));
  p->shape.setPosition(position);
  p->shape.setSize(dimension);
  p->shape.setFillColor(colour);

  return(p);
}

#endif
