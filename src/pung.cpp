#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cmath>
#include <random>
#include <algorithm>
#include <cmath>
#include <stdlib.h>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <tclap/CmdLine.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 550
#define SCALE 30.f
#define PI 3.14159
#define E 2.71828182845904523536

namespace ublas = boost::numeric::ublas;
typedef ublas::matrix<float> matrix;
typedef ublas::vector<float> vector;

/*
 * Collect data:
 *  - Distance between ball and paddle
 *  - Linear velocity of ball
 *  - Direction of ball travel [-1 away, 1 toward] (?)
 *  - Difference between paddle Y and ball Y
 *  - Output: target value
 *
 * Learn from data:
 *  - Use R
 *  - Standard backprop. Worry about it not working later
 */

struct paddle
{
  sf::RectangleShape *shape;
  sf::Vector2f pos;
  sf::Vector2f dim;
  sf::Color colour;
  b2Body *body;
  float target_y;
  int score = 0;
};

struct ball
{
  b2Body *body;
  sf::CircleShape *shape;
  sf::Color colour;
  float radius;
  float speed;
};

inline vector
sigmoid(vector z)
{
  for (unsigned long i=0; i < z.size(); ++i)
  {
    z(i) = (1.f / (1.f + std::pow(E, -z(i))));
  }
  return z;
}

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
  std::ofstream human_file;
  std::ifstream ai_file;

  const std::string delim = ",";

  std::string filepath;
  std::string filename;
  int playmode;
  int hidden_nodes;
  sf::Color opponent_colour;

  float framerate = 60.f;
  int inputs = 5;

  try
  {
    TCLAP::CmdLine cmd("Test", ' ', "0.01");
    TCLAP::ValueArg<std::string> filenameArg(
      "f",
      "file",
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

    TCLAP::ValueArg<int> hiddenNodesArg(
      "n",
      "hidden-nodes",
      "Hidden nodes",
      false,
      0,
      "int");

    cmd.add(filenameArg);
    cmd.add(playmodeArg);
    cmd.add(hiddenNodesArg);
    cmd.parse(argc, argv);
    filename = filenameArg.getValue();
    playmode = playmodeArg.getValue();
    hidden_nodes = hiddenNodesArg.getValue();

    switch(playmode)
    {
      case 0:
        filepath = "data/human/";
        std::cout << "Collecting human data..." << "\n";
        std::cout << "Writing to " << filepath << filename << "\n";
        human_file.open(filepath + filename, std::ios_base::app);
        opponent_colour = sf::Color::Green;
        break;
      case 1:
        filepath = "data/ai/";
        std::cout << "Playing against AI" << "\n";
        std::cout << "Reading from " << filepath << filename << "\n";
        ai_file.open(filepath + filename);
        opponent_colour = sf::Color::Red;
        break;
      default:
        std::cerr << "Unknown playmode!" << "\n";
        exit(EXIT_FAILURE);
    }
  }
  catch (TCLAP::ArgException &e)
  {
    std::cerr << "ERROR: " << e.error() << " " << e.argId() << std::endl;
    exit(EXIT_FAILURE);
  }

  int weights_length = std::count (std::istreambuf_iterator<char>(ai_file),
                                   std::istreambuf_iterator<char>(), '\n');
  ai_file.clear();
  ai_file.seekg(0, std::ios::beg);

  vector Weights(weights_length);
  matrix Theta1(inputs,hidden_nodes);
  matrix Theta2(hidden_nodes+1,1);

  vector h1(hidden_nodes+1);

  if (playmode == 1)
  {
    std::string line;
    size_t index = 0;
    while(getline(ai_file, line))
    {
      float w = std::stof(line);
      Weights(index) = w;
      ++index;
    }
    ai_file.close();

    //split theta
    int w_index = 0;
    for (int i=0; i < hidden_nodes; ++i)
    {
      for(int k=0; k < inputs; ++k)
      {
        Theta1(k,i) = Weights(w_index);
        ++w_index;
      }
    }

    for(int j=0; j < hidden_nodes; ++j)
    {
      Theta2(j,0) = Weights(w_index);
      ++w_index;
    }
  }

  // Do window stuff
  sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32), "PUNG");
  window.setMouseCursorVisible(false);
  window.setFramerateLimit(60);

  b2Vec2 Gravity(0.0f, 0.0f);
  b2World World(Gravity);

  // Create player paddle
  paddle* p1 = (paddle*)malloc(sizeof(paddle));
  p1->dim = sf::Vector2f(20,100);
  p1->pos = sf::Vector2f(
    50,
    (SCREEN_HEIGHT / 2) - (p1->pos.y / 2));
  p1->colour = sf::Color::Yellow;
  p1->score = 0;

  b2BodyDef p1_p1_paddle_body_def;
  p1_p1_paddle_body_def.position = b2Vec2(p1->pos.x/SCALE, p1->pos.y/SCALE);
  p1_p1_paddle_body_def.type = b2_staticBody;
  b2Body* p1_paddle_body = World.CreateBody(&p1_p1_paddle_body_def);
  p1->body = p1_paddle_body;

  b2PolygonShape p1_b2_paddle_shape;
  p1_b2_paddle_shape.SetAsBox((p1->dim.x/2)/SCALE, (p1->dim.y/2)/SCALE);

  b2FixtureDef p1_b2_paddle_fixture_def;
  p1_b2_paddle_fixture_def.shape = &p1_b2_paddle_shape;
  p1_b2_paddle_fixture_def.density = 1.f;
  p1->body->CreateFixture(&p1_b2_paddle_fixture_def);

  sf::RectangleShape p1_shape;
  p1->shape = &p1_shape;
  p1->shape->setOrigin(sf::Vector2f(p1->dim.x/2, p1->dim.y/2));
  p1->shape->setPosition(sf::Vector2f(p1->pos.x, p1->pos.y));
  p1->shape->setSize(sf::Vector2f(p1->dim.x, p1->dim.y));
  p1->shape->setFillColor(p1->colour);


  // Create AI paddle
  paddle* p2 = (paddle*)malloc(sizeof(paddle));
  p2->dim = sf::Vector2f(20,100);
  p2->pos = sf::Vector2f(
    (SCREEN_WIDTH - 50),
    (SCREEN_HEIGHT / 2)
  );
  p2->colour = opponent_colour;
  p2->score = 0;

  b2BodyDef paddle_body_def;
  paddle_body_def.position = b2Vec2(p2->pos.x/SCALE, p2->pos.y/SCALE);
  paddle_body_def.type = b2_staticBody;
  b2Body* paddle_body = World.CreateBody(&paddle_body_def);
  p2->body = paddle_body;

  b2PolygonShape b2_paddle_shape;
  b2_paddle_shape.SetAsBox((p2->dim.x/2)/SCALE, (p2->dim.y/2)/SCALE);

  b2FixtureDef b2_paddle_fixture_def;
  b2_paddle_fixture_def.shape = &b2_paddle_shape;
  b2_paddle_fixture_def.density = 1.f;
  p2->body->CreateFixture(&b2_paddle_fixture_def);

  sf::RectangleShape p2_shape;
  p2->shape = &p2_shape;
  p2->shape->setOrigin(sf::Vector2f(p2->dim.x/2, p2->dim.y/2));
  p2->shape->setPosition(sf::Vector2f(p2->pos.x, p2->pos.y));
  p2->shape->setSize(sf::Vector2f(p2->dim.x, p2->dim.y));
  p2->shape->setFillColor(p2->colour);


  // Create divider
  sf::RectangleShape divider;
  divider.setSize(sf::Vector2f(10, SCREEN_HEIGHT));
  divider.setPosition(sf::Vector2f((SCREEN_WIDTH / 2) + 5, 0));
  divider.setFillColor(sf::Color(200,200,200));

  // Create ball
  ball* b = (ball*)malloc(sizeof(ball));
  b->radius = 20.0f;
  b->speed = 25.f;
  b->colour = sf::Color::Red;

  sf::Texture ball_texture;
  ball_texture.loadFromFile("assets/images/ball.png");

  sf::CircleShape ball_shape;
  b->shape = &ball_shape;
  b->shape->setRadius(b->radius);
  b->shape->setOrigin(sf::Vector2f((b->radius),(b->radius)));
  b->shape->setTexture(&ball_texture);

  b2BodyDef ball_body_def;
  ball_body_def.position = b2Vec2(10/SCALE,10/SCALE);
  ball_body_def.type = b2_dynamicBody;

  b2Body* ball_body = World.CreateBody(&ball_body_def);
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

  // create paddle score text things
  sf::Font score_font;
  if(!score_font.loadFromFile("assets/fonts/Precursive.otf"))
  {
    std::cerr << "Font failed to load!" << "\n";
    return(EXIT_FAILURE);
  }

  sf::Text p1_score;
  p1_score.setFont(score_font);
  p1_score.setString(std::to_string(p1->score));
  p1_score.setCharacterSize(32);
  p1_score.setPosition((SCREEN_WIDTH/2) - 50,10);
  p1_score.setOrigin(p1_score.getScale()/2.f);

  sf::Text p2_score;
  p2_score.setFont(score_font);
  p2_score.setString(std::to_string(p2->score));
  p2_score.setCharacterSize(32);
  p2_score.setPosition((SCREEN_WIDTH/2) + 50,10);
  p2_score.setOrigin(p2_score.getScale()/2.f);

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

  float damping = 0.7f;

  while (window.isOpen())
  {
    World.Step(1.f/framerate, 8, 3);

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

    b->shape->setPosition(
      SCALE * b->body->GetPosition().x,
      SCALE * b->body->GetPosition().y);

    float dist;
    b2Vec2 ball_v = b->body->GetLinearVelocity();
    float ball_y = ((b->body->GetPosition().y) / (SCREEN_HEIGHT/SCALE)) * 100;
    float mouse_y = sf::Mouse::getPosition(window).y;

    if (mouse_y >= p1->dim.y / 2 && mouse_y <= SCREEN_HEIGHT - (p1->dim.y / 2))
      p1->target_y = sf::Mouse::getPosition(window).y / SCALE;

    b2Vec2 p1_position = p1->body->GetPosition();
    p1_position.y = p1->target_y - (p1->target_y - p1_position.y) * damping;
    p1->body->SetTransform(p1_position, 0);

    p1->shape->setPosition(
      SCALE * p1->body->GetPosition().x,
      SCALE * p1->body->GetPosition().y);

    b2Vec2 p2_position = p2->body->GetPosition();

    if (playmode == 1)
    {
      dist = std::abs(b->body->GetPosition().x - p2->body->GetPosition().x);
      vector a1(inputs);
      a1(0) = 1;
      a1(1) = dist;
      a1(2) = ball_y;
      a1(3) = ball_v.x;
      a1(4) = ball_v.y;

      vector z2 = prod(a1,Theta1);

      vector tmp = sigmoid(z2);
      vector a2(hidden_nodes+1);

      a2(0) = 1;
      for (unsigned long i=1; i < a2.size(); ++i)
      {
        a2(i) = tmp(i-1);
      }

      vector h = sigmoid(prod(a2, Theta2));

      float p2_target = b->body->GetPosition().y;
      p2_target = (h(0) * SCREEN_HEIGHT/SCALE);
      p2_position.y = p2_target - (p2_target - p2_position.y) * damping;

      //draw neural net
    }
    else
    {
      dist = std::abs(b->body->GetPosition().x - p1->body->GetPosition().x);

      p2_position = p2->body->GetPosition();
      float p2_target = b->body->GetPosition().y;
      p2_position.y = p2_target - (p2_target - p2_position.y) * damping;
    }

    p2->body->SetTransform(p2_position, 0);

    p2->shape->setPosition(
      SCALE * p2->body->GetPosition().x,
      SCALE * p2->body->GetPosition().y);

    float vert_velocity = b->body->GetLinearVelocity().y;
    if (std::abs(vert_velocity) < 3.f)
    {
      float new_vert_velocity;
      new_vert_velocity = (vert_velocity < 0) ? -2.f : 2.f;
      b2Vec2 ball_vertical_velocity = b2Vec2(0.f, new_vert_velocity);
      b->body->ApplyForceToCenter(ball_vertical_velocity, true);
    }

    float horiz_velocity = b->body->GetLinearVelocity().x;
    if (std::abs(horiz_velocity) < b->speed)
    {
      float new_horiz_velocity;
      new_horiz_velocity = (horiz_velocity < 0) ? -5.f : 5.f;
      b2Vec2 ball_horizical_velocity = b2Vec2(new_horiz_velocity, 0.f);
      b->body->ApplyForceToCenter(ball_horizical_velocity, true);
    }

    if (b->body->GetPosition().x < 0 || b->body->GetPosition().x > (SCREEN_WIDTH / SCALE))
    {
      if (b->body->GetPosition().x < 0)
        p2->score += 1;
      else
        p1->score += 1;

      float choices[2] = {-1.f, 1.f};
      std::srand(time(nullptr));
      int r = rand()%2;
      float c = choices[r];
      b->body->SetTransform(b2Vec2((SCREEN_WIDTH/2)/SCALE, (SCREEN_HEIGHT/2)/SCALE), 0);
      b->body->SetLinearVelocity(b2Vec2(
        b->speed * c,
        random_float(-b->speed, b->speed)));
    }

    //b->body->SetAngularVelocity(0.f);
    b->shape->setRotation(b->body->GetAngle() * (180 / PI));

    // write human data to file
    if (playmode == 0)
    {
      float target_y = (p1->target_y / (SCREEN_HEIGHT/SCALE)) * 100.f;

      std::ostringstream buff;
      buff << dist << delim << ball_y << delim << ball_v.x << delim << ball_v.y << delim << target_y << "\n";
      human_file << buff.str();
    }

    p1_score.setString(std::to_string(p1->score));
    p2_score.setString(std::to_string(p2->score));

    window.clear(sf::Color(110,110,110));
    window.draw(p1_score);
    window.draw(p2_score);
    window.draw(divider);
    window.draw(*p1->shape);
    window.draw(*p2->shape);
    window.draw(*b->shape);
    window.display();

    dt = clk.restart();
  }

  free(p1);
  free(p2);
  free(b);

  if (human_file.is_open())
    human_file.close();

  return EXIT_SUCCESS;
}
