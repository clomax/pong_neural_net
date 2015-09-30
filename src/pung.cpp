#include <iostream>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 550
#define METERS_PER_PIXEL 5
#define PIXELS_PER_METER 1 / METERS_PER_PIXEL

struct paddle
{
  sf::RectangleShape *shape;
  sf::Vector2f pos;
  sf::Vector2f dim;
  sf::Color colour;
};

struct ball
{
  sf::CircleShape *shape;
  b2Body *body;
  sf::Color colour;
  sf::Vector2f pos;
  float speed;
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
  paddle *p1 = (paddle *) malloc(sizeof(paddle));
  p1->dim = sf::Vector2f(20,100);
  p1->pos = sf::Vector2f(50, (SCREEN_HEIGHT / 2) - (p1->dim.y / 2));
  p1->colour = sf::Color::Yellow;

  sf::RectangleShape p1_shape;
  p1->shape = &p1_shape;
  p1->shape->setSize(sf::Vector2f(p1->dim.x, p1->dim.y));
  p1->shape->setFillColor(p1->colour);


  // Create AI paddle
  paddle *p2 = (paddle *) malloc(sizeof(paddle));
  p2->dim = sf::Vector2f(20, 100);
  p2->pos = sf::Vector2f((SCREEN_WIDTH - 50), (SCREEN_HEIGHT / 2) - (p2->dim.y / 2));
  p2->colour = sf::Color::Yellow;

  sf::RectangleShape p2_shape;
  p2->shape = &p2_shape;
  p2->shape->setSize(sf::Vector2f(p2->dim.x, p2->dim.y));
  p2->shape->setFillColor(p1->colour);


  // Create divider
  sf::RectangleShape divider;
  divider.setSize(sf::Vector2f(10, SCREEN_HEIGHT));
  divider.setPosition(sf::Vector2f((SCREEN_WIDTH / 2) + 5, 0));


  // Create ball 0
  ball *ball0 = (ball *) malloc(sizeof(ball));
  ball0->radius = 20.0f;
  ball0->pos = sf::Vector2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
  ball0->colour = sf::Color::Red;

  b2BodyDef ball_body_def;

  ball_body_def.position = b2Vec2(
    (SCREEN_WIDTH / 2) * PIXELS_PER_METER,
    (SCREEN_HEIGHT / 2)
  );

  ball_body_def.type = b2_dynamicBody;
  b2Body* ball_body = World.CreateBody(&ball_body_def);
  ball0->body = ball_body;

  b2CircleShape b2_ball_shape;
  b2_ball_shape.m_p.Set(0,0);
  b2_ball_shape.m_radius = ball0->radius;

  b2FixtureDef b2_ball_fixture_def;
  b2_ball_fixture_def.shape = &b2_ball_shape;
  b2_ball_fixture_def.density = 100.f;
  ball0->body->CreateFixture(&b2_ball_fixture_def);

  sf::CircleShape ball_shape;
  ball0->shape = &ball_shape;
  ball0->shape->setRadius(ball0->radius);
  ball0->shape->setOrigin(sf::Vector2f(ball0->radius/2, ball0->radius/2));
  ball0->shape->setPosition(SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
  ball0->shape->setFillColor(ball0->colour);

  b2Vec2 vel = ball0->body->GetLinearVelocity();
  vel.x = -50 * METERS_PER_PIXEL;
  ball0->body->SetLinearVelocity(vel);

  sf::Clock clk;
  sf::Time dt;

  while (window.isOpen())
  {
    dt = clk.restart();

    window.clear(sf::Color(128,128,128));
    World.Step(1/60.f, 8, 3);

    sf::Event event;
    while(window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
      if (p1->pos.y >= 0)
        p1->pos.y -= 350.0f * dt.asSeconds();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
      if (p1->pos.y <= (SCREEN_HEIGHT - p1->dim.y))
        p1->pos.y += 350.0f * dt.asSeconds();
    }

    p1->shape->setPosition(p1->pos.x, p1->pos.y);
    p2->shape->setPosition(p2->pos.x, p2->pos.y);

    ball0->shape->setPosition(
      ball0->body->GetPosition().x * METERS_PER_PIXEL,
      ball0->body->GetPosition().y
    );

    if(ball0->body->GetPosition().x < 0)
    {
      ball0->body->SetTransform(b2Vec2(
        SCREEN_WIDTH/2 * PIXELS_PER_METER,
        SCREEN_HEIGHT/2),
        0
      );
      b2Vec2 vel = b2Vec2(50*METERS_PER_PIXEL,0);
      ball0->body->SetLinearVelocity(vel);
    }

    if(ball0->body->GetPosition().x > SCREEN_WIDTH * PIXELS_PER_METER)
    {
      ball0->body->SetTransform(b2Vec2(
        SCREEN_WIDTH/2 * PIXELS_PER_METER,
        SCREEN_HEIGHT/2),
        0
      );
      b2Vec2 vel = b2Vec2(-50*METERS_PER_PIXEL,0);
      ball0->body->SetLinearVelocity(vel);
    }

    //ball0->body->ApplyForceToCenter( b2Vec2(-500,0), true );

    window.draw(*p1->shape);
    window.draw(*p2->shape);
    window.draw(divider);
    window.draw(*ball0->shape);
    window.display();
  }

  return EXIT_SUCCESS;
}
