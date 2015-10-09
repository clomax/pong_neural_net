#include <iostream>
#include <string>
#include <random>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <tclap/CmdLine.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 550
#define SCALE 30.f
#define PI 3.14159

/*
 * Add arguments:
 *  - Collect data + data filename
 *  - Play against NN + weights filename
 *
 * Collect data:
 *  - Distance between ball and paddle (absolute)
 *  - Linear velocity of ball (absolute)
 *  - Difference between paddle Y and ball Y
 *  - Output: target value
 */

struct paddle
{
  std::string name;
  sf::RectangleShape *shape;
  sf::Vector2f pos;
  sf::Vector2f dim;
  sf::Color colour;
  b2Body *body;
  float speed;
  float target_y;
};

struct ball
{
  std::string name;
  b2Body *body;
  sf::CircleShape *shape;
  sf::Color colour;
  float radius;
  float speed;
};

inline float
random_float (float min, float max)
{
  float random = ((float) rand()) / (float) RAND_MAX;
  float range = max - min;
  return (random*range) + min;
}

int
main (int argc, char ** argv)
{

  try
  {
    TCLAP::CmdLine cmd("Test", ' ', "0.01");
    TCLAP::ValueArg<std::string> filenameArg(
      "f",
      "filename",
      "File name",
      true,
      "/dev/null",
      "string");

    TCLAP::ValueArg<int> playmodeArg(
      "p",
      "playmode",
      "Play mode",
      true,
      0,
      "int");

    cmd.add(filenameArg);
    cmd.add(playmodeArg);
    cmd.parse(argc, argv);
    std::string filename = filenameArg.getValue();
    int playmode = playmodeArg.getValue();

    switch(playmode)
    {
      case 0:
        std::cout << "Collecting human data..." << "\n";
        std::cout << "Writing to " << filename << "\n";
        break;
      case 1:
        std::cout << "Playing against AI" << "\n";
        std::cout << "Reading from " << filename << "\n";
        break;
      default:
        std::cout << "Undefined playmode!" << "\n";
        return EXIT_SUCCESS;
    }
  }
  catch (TCLAP::ArgException &e)
  {
    std::cerr << "Error!\n";
  }

  return 0;

  sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32), "PUNG");
  //window.setMouseCursorVisible(false);
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
    (SCREEN_HEIGHT / 2)
  );
  p2.colour = sf::Color::Yellow;
  p2.speed = 1.f;

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
  ball0.radius = 20.0f;
  ball0.speed = 25.f;
  ball0.colour = sf::Color::Red;

  sf::Texture ball_texture;
  ball_texture.loadFromFile("assets/ball.png");

  sf::CircleShape ball_shape;
  ball0.shape = &ball_shape;
  ball0.shape->setRadius(ball0.radius);
  ball0.shape->setOrigin(sf::Vector2f((ball0.radius),(ball0.radius)));
  ball0.shape->setTexture(&ball_texture);

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
  ball0.body->SetLinearVelocity(b2Vec2(-10.f,-7.f));

  // Create walls
  b2BodyDef w0_body_def;
  w0_body_def.position = b2Vec2((SCREEN_WIDTH/2)/SCALE, 0);
  w0_body_def.type = b2_staticBody;
  b2Body* w0_body = World.CreateBody(&w0_body_def);

  b2PolygonShape b2_wall_shape;
  b2_wall_shape.SetAsBox((SCREEN_WIDTH/SCALE), 1/SCALE);

  b2FixtureDef b2_wall_fixture_def;
  b2_wall_fixture_def.shape = &b2_wall_shape;
  b2_wall_fixture_def.density = 1.f;
  w0_body->CreateFixture(&b2_wall_fixture_def);


  b2BodyDef w1_body_def;
  w1_body_def.position = b2Vec2((SCREEN_WIDTH/2)/SCALE, SCREEN_HEIGHT/SCALE);
  w1_body_def.type = b2_staticBody;
  b2Body* w1_body = World.CreateBody(&w1_body_def);

  b2PolygonShape b2_wall1_shape;
  b2_wall1_shape.SetAsBox((SCREEN_WIDTH/SCALE), 1/SCALE);

  b2FixtureDef b2_wall1_fixture_def;
  b2_wall_fixture_def.shape = &b2_wall1_shape;
  b2_wall_fixture_def.density = 1.f;
  w1_body->CreateFixture(&b2_wall_fixture_def);

  sf::Mouse::setPosition(sf::Vector2i(SCREEN_WIDTH/2, SCREEN_HEIGHT/2), window);

  sf::Clock clk;
  sf::Time dt;

  float damping = 0.9f;

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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
      window.close();
    }

    float mouse_y = sf::Mouse::getPosition(window).y;
    if (mouse_y >= p1.dim.y / 2 && mouse_y <= SCREEN_HEIGHT - (p1.dim.y / 2))
      p1.target_y = sf::Mouse::getPosition(window).y / SCALE;

    b2Vec2 p1_position = p1.body->GetPosition();
    p1_position.y = p1.target_y - (p1.target_y - p1_position.y) * 0.f;
    p1.body->SetTransform(p1_position, 0);

    p1.shape->setPosition(
      SCALE * p1.body->GetPosition().x,
      SCALE * p1.body->GetPosition().y);

    b2Vec2 p2_position = p2.body->GetPosition();
    float p2_target = ball0.body->GetPosition().y;
    p2_position.y = p2_target - (p2_target - p2_position.y) * damping;
    p2.body->SetTransform(p2_position, 0);

    p2.shape->setPosition(
      SCALE * p2.body->GetPosition().x,
      SCALE * p2.body->GetPosition().y);


    ball0.shape->setPosition(
      SCALE * ball0.body->GetPosition().x,
      SCALE * ball0.body->GetPosition().y);

    float vert_velocity = ball0.body->GetLinearVelocity().y;
    if (std::abs(vert_velocity) < 3.f)
    {
      float new_vert_velocity;
      new_vert_velocity = (vert_velocity < 0) ? -5.f : 5.f;
      b2Vec2 ball_vertical_velocity = b2Vec2(
        ball0.body->GetLinearVelocity().x,
        new_vert_velocity);
      ball0.body->SetLinearVelocity(ball_vertical_velocity);
    }

    float horiz_velocity = ball0.body->GetLinearVelocity().x;
    if (std::abs(horiz_velocity) < ball0.speed)
    {
      float new_horiz_velocity;
      new_horiz_velocity = (horiz_velocity < 0) ? -ball0.speed : ball0.speed;
      b2Vec2 ball_horizical_velocity = b2Vec2(new_horiz_velocity, ball0.body->GetLinearVelocity().y);
      ball0.body->SetLinearVelocity(ball_horizical_velocity);
    }

    if (ball0.body->GetPosition().x < 0 || ball0.body->GetPosition().x > (SCREEN_WIDTH / SCALE))
    {
      ball0.body->SetTransform(b2Vec2((SCREEN_WIDTH/2)/SCALE, (SCREEN_HEIGHT/2)/SCALE), 0);
      ball0.body->SetLinearVelocity(b2Vec2(random_float(-10.f, 10.f), random_float(-7.f, 7.f)));
    }

    ball0.shape->setRotation(ball0.body->GetAngle() * (180/PI));


    for (b2Contact* contact = World.GetContactList();
         contact;
         contact = contact->GetNext())
    {
    }

    window.clear(sf::Color(110,110,110));

    window.draw(divider);
    window.draw(*p1.shape);
    window.draw(*p2.shape);
    window.draw(*ball0.shape);
    window.display();
  }

  return EXIT_SUCCESS;
}
