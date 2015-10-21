#ifndef BALL_HPP
#define BALL_HPP

#include <memory>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "util.hpp"

struct ball
{
  b2Body *body;
  sf::CircleShape shape;
  sf::Color colour;
  float radius;
  float speed;
};

std::unique_ptr<ball>
create_ball(b2World* world, float radius)
{
  std::unique_ptr<ball> b(new ball);
  b->radius = radius;
  b->speed = 25.f;


  sf::CircleShape ball_shape;
  b->shape = ball_shape;
  b->shape.setRadius(b->radius);
  b->shape.setOrigin(sf::Vector2f((b->radius),(b->radius)));

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

  return(b);
}

#endif
