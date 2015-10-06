#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 550
#define SCALE 30.f

struct paddle
{
  std::string name;
  sf::RectangleShape *shape;
  sf::Vector2f pos;
  sf::Vector2f dim;
  sf::Color colour;
  b2Body *body;
};

struct ball
{
  std::string name;
  b2Body *body;
  sf::CircleShape *shape;
  sf::Color colour;
  float radius;
};

int
main (int argc, char ** argv)
{
  sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32), "PUNG");
  window.setFramerateLimit(60);

  b2Vec2 Gravity(0.0f, 0.0f);
  b2World World(Gravity);

  // Create player paddle
  paddle p1;
  p1.name = std::string("Paddle 1");
  p1.dim = sf::Vector2f(20,100);
  p1.pos = sf::Vector2f(
    50,
    (SCREEN_HEIGHT / 2) - (p1.pos.y / 2));
  p1.colour = sf::Color::Yellow;

  b2BodyDef p1_p1_paddle_body_def;
  p1_p1_paddle_body_def.position = b2Vec2(p1.pos.x/SCALE, p1.pos.y/SCALE);
  p1_p1_paddle_body_def.type = b2_staticBody;
  b2Body* p1_paddle_body = World.CreateBody(&p1_p1_paddle_body_def);
  p1.body = p1_paddle_body;

  b2PolygonShape p1_b2_paddle_shape;
  p1_b2_paddle_shape.SetAsBox((p1.dim.x/2)/SCALE, (p1.dim.y/2)/SCALE);

  b2FixtureDef p1_b2_paddle_fixture_def;
  p1_b2_paddle_fixture_def.shape = &p1_b2_paddle_shape;
  p1_b2_paddle_fixture_def.density = 1.f;
  p1.body->CreateFixture(&p1_b2_paddle_fixture_def);

  sf::RectangleShape p1_shape;
  p1.shape = &p1_shape;
  p1.shape->setOrigin(sf::Vector2f(p1.dim.x/2, p1.dim.y/2));
  p1.shape->setPosition(sf::Vector2f(p1.pos.x, p1.pos.y));
  p1.shape->setSize(sf::Vector2f(p1.dim.x, p1.dim.y));
  p1.shape->setFillColor(p1.colour);


  // Create AI paddle
  paddle p2;
  p2.name = std::string("Paddle 2");
  p2.dim = sf::Vector2f(20,100);
  p2.pos = sf::Vector2f(
    (SCREEN_WIDTH - 50),
    (SCREEN_HEIGHT / 2) - (p2.dim.y / 2)
  );
  p2.colour = sf::Color::Yellow;

  b2BodyDef paddle_body_def;
  paddle_body_def.position = b2Vec2(p2.pos.x/SCALE, p2.pos.y/SCALE);
  paddle_body_def.type = b2_staticBody;
  b2Body* paddle_body = World.CreateBody(&paddle_body_def);
  p2.body = paddle_body;

  b2PolygonShape b2_paddle_shape;
  b2_paddle_shape.SetAsBox((p2.dim.x/2)/SCALE, (p2.dim.y/2)/SCALE);

  b2FixtureDef b2_paddle_fixture_def;
  b2_paddle_fixture_def.shape = &b2_paddle_shape;
  b2_paddle_fixture_def.density = 1.f;
  p2.body->CreateFixture(&b2_paddle_fixture_def);

  sf::RectangleShape p2_shape;
  p2.shape = &p2_shape;
  p2.shape->setOrigin(sf::Vector2f(p2.dim.x/2, p2.dim.y/2));
  p2.shape->setPosition(sf::Vector2f(p2.pos.x, p2.pos.y));
  p2.shape->setSize(sf::Vector2f(p2.dim.x, p2.dim.y));
  p2.shape->setFillColor(p2.colour);


  // Create divider
  sf::RectangleShape divider;
  divider.setSize(sf::Vector2f(10, SCREEN_HEIGHT));
  divider.setPosition(sf::Vector2f((SCREEN_WIDTH / 2) + 5, 0));
  divider.setFillColor(sf::Color(200,200,200));

  // Create ball 0
  ball ball0;
  ball0.name = "Ball";
  ball0.radius = 5.0f;
  ball0.colour = sf::Color::Red;

  sf::CircleShape ball_shape;
  ball0.shape = &ball_shape;
  ball0.shape->setRadius(ball0.radius);
  ball0.shape->setOrigin(sf::Vector2f(ball0.radius/2, ball0.radius/2));
  ball0.shape->setFillColor(ball0.colour);

  b2BodyDef ball_body_def;
  ball_body_def.position = b2Vec2(10/SCALE,10/SCALE);
  ball_body_def.type = b2_dynamicBody;

  b2Body* ball_body = World.CreateBody(&ball_body_def);
  ball0.body = ball_body;

  ball_body->SetUserData(&ball0);

  b2CircleShape b2_ball_shape;
  b2_ball_shape.m_radius = ball0.radius/SCALE;

  b2FixtureDef b2_ball_fixture_def;
  b2_ball_fixture_def.shape = &b2_ball_shape;
  b2_ball_fixture_def.density = 1.f;
  b2_ball_fixture_def.restitution = 1.f;
  ball0.body->CreateFixture(&b2_ball_fixture_def);
  ball0.body->SetTransform(b2Vec2((SCREEN_WIDTH/2)/SCALE, (SCREEN_HEIGHT/2)/SCALE), 0);

  ball0.body->SetLinearVelocity(b2Vec2(-10.f,0.f));

  sf::Clock clk;
  sf::Time dt;

  while (window.isOpen())
  {
    dt = clk.restart();
    World.Step(1/60.f, 8, 3);

    sf::Event event;
    while(window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    b2Vec2 p1_position = p1.body->GetPosition();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
      if (p1_position.y >= (p1.dim.y/2) / SCALE)
      {
        p1_position.y -= 5.f * dt.asSeconds();
        p1.body->SetTransform(p1_position, 0);
      }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
      if (p1_position.y <= (SCREEN_HEIGHT / SCALE) - ((p1.dim.y/2) / SCALE))
      {
        p1_position.y += 5.f * dt.asSeconds();
        p1.body->SetTransform(p1_position, 0);

      }
    }

    window.clear(sf::Color(128,128,128));

    p1.shape->setPosition(
      SCALE * p1.body->GetPosition().x,
      SCALE * p1.body->GetPosition().y);

    ball0.shape->setPosition(
      SCALE * ball0.body->GetPosition().x,
      SCALE * ball0.body->GetPosition().y);

    for (b2Body* BodyIterator = World.GetBodyList();
         BodyIterator !=0;
         BodyIterator = BodyIterator->GetNext())
    {

    }

    window.draw(divider);
    window.draw(*p1.shape);
    window.draw(*p2.shape);
    window.draw(*ball0.shape);
    window.display();
  }

  return EXIT_SUCCESS;
}
