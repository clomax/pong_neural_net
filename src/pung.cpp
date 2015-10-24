#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cmath>
#include <random>
#include <algorithm>
#include <memory>
#include <cmath>
#include <stdlib.h>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <tclap/CmdLine.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "entity.hpp"
#include "util.hpp"

namespace ublas = boost::numeric::ublas;
typedef ublas::matrix<float> matrix;
typedef ublas::vector<float> vector;


inline vector
sigmoid(vector z)
{
  for (unsigned long i=0; i < z.size(); ++i)
  {
    z(i) = (1.f / (1.f + std::pow(E, -z(i))));
  }
  return z;
}

inline void
asset_load_err (std::string asset)
{
  std::cerr << "Failed to load: " << asset << std::endl;
  exit(EXIT_FAILURE);
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
        break;
      case 1:
        filepath = "data/ai/";
        std::cout << "Playing against AI" << "\n";
        std::cout << "Reading from " << filepath << filename << "\n";
        ai_file.open(filepath + filename);
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

  std::vector<entity*> entities;

  sf::Texture paddle_texture;
  paddle_texture.loadFromFile("assets/images/paddle.png");

  // Create player paddle
  sf::Vector2f pos = sf::Vector2f(50, (SCREEN_HEIGHT/2));
  sf::Vector2f dim = sf::Vector2f(20,100);
  sf::Color col = sf::Color::Yellow;
  sf::Sprite p1_sprite;

  entity* p1 = (entity*)malloc(sizeof(entity));
  p1->type = entity_paddle;
  p1->component_flags |=
    component_sprite|component_rigidbody|component_position|component_dimensions|component_colour;
  build_entity(p1, &p1_sprite, pos, dim, paddle_texture, col);
  entities.push_back(p1);

  // Create AI paddle
  pos = sf::Vector2f((SCREEN_WIDTH-50), (SCREEN_HEIGHT/2));
  col = (playmode == 0) ? sf::Color::Green : sf::Color::Red;
  sf::Sprite p2_sprite;

  entity* p2 = (entity*)malloc(sizeof(entity));
  p2->type = entity_paddle;
  p2->component_flags |=
    component_sprite|component_rigidbody|component_position|component_dimensions|component_colour;
  p2->sprite = &p2_sprite;
  p2->sprite->setPosition(pos);
  p2->sprite->setColor(col);
  p2->sprite->setTexture(paddle_texture);
  p2->sprite->setTextureRect(sf::IntRect(0,0,dim.x,dim.y));
  p2->sprite->setOrigin(sf::Vector2f(dim.x/2, dim.y/2));
  entities.push_back(p2);


  dim = sf::Vector2f(50,50);
  pos = sf::Vector2f((SCREEN_WIDTH/2),(SCREEN_HEIGHT/2));
  sf::Texture ball_texture;
  ball_texture.loadFromFile("assets/images/ball.png");

  sf::Sprite ball_sprite;
  entity* ball = (entity*)malloc(sizeof(entity));
  ball->type = entity_ball;
  ball->component_flags |=
    component_sprite|component_rigidbody|component_position;
  ball->sprite = &ball_sprite;
  ball->sprite->setPosition(pos);
  ball->sprite->setTexture(ball_texture);
  ball->sprite->setTextureRect(sf::IntRect(0,0,dim.x,dim.y));
  ball->sprite->setOrigin(sf::Vector2f(dim.x/2, dim.y/2));
  entities.push_back(ball);

  while (window.isOpen())
  {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
      window.close();

    window.clear(sf::Color(110,110,110));
    window.draw(*(p1->sprite));
    window.draw(*(p2->sprite));
    window.draw(*(ball->sprite));
    window.display();
  }

  return EXIT_SUCCESS;
}
/*
  // Create AI paddle
  pos = sf::Vector2f((SCREEN_WIDTH-50), (SCREEN_HEIGHT/2));
  dim = sf::Vector2f(20,100);
  col = opponent_colour;
  std::unique_ptr<paddle> p2 = create_paddle(&World, pos, dim, col);

  // Create divider
  sf::RectangleShape divider;
  divider.setSize(sf::Vector2f(10, SCREEN_HEIGHT));
  divider.setPosition(sf::Vector2f((SCREEN_WIDTH / 2) + 5, 0));
  divider.setFillColor(sf::Color(200,200,200));

  // Create ball
  float rad = 20.f;
  sf::Texture ball_texture;
  ball_texture.loadFromFile("assets/images/ball.png");
  std::unique_ptr<ball> b = create_ball(&World, rad);
  b->shape.setTexture(&ball_texture);

  // create paddle score text things
  sf::Font score_font;
  std::string font_file = "assets/fonts/Precursive.otf";
  if(!score_font.loadFromFile(font_file))
  {
    asset_load_err(font_file);
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

    b->shape.setPosition(
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

    p1->shape.setPosition(
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
    }
    else
    {
      dist = std::abs(b->body->GetPosition().x - p1->body->GetPosition().x);

      p2_position = p2->body->GetPosition();
      float p2_target = b->body->GetPosition().y;
      p2_position.y = p2_target - (p2_target - p2_position.y) * damping;
    }

    p2->body->SetTransform(p2_position, 0);

    p2->shape.setPosition(
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

    b->shape.setRotation(b->body->GetAngle() * (180 / PI));

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
    window.draw(p1->shape);
    window.draw(p2->shape);
    window.draw(b->shape);
    window.display();

    dt = clk.restart();
  }

  if (human_file.is_open())
    human_file.close();
  return EXIT_SUCCESS;
}
*/
