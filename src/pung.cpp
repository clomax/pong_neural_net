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
  //window.setMouseCursorVisible(false);
  window.setFramerateLimit(60);

  b2Vec2 Gravity(0.f, 0.f);
  b2World World(Gravity);

  std::vector<entity*> entities;

  sf::Texture blank_texture;
  blank_texture.loadFromFile("assets/images/blank.png");

  sf::Texture ball_texture;
  ball_texture.loadFromFile("assets/images/ball.png");

  // Create player paddle
  sf::Sprite p1_sprite;
  entity* p1 = (entity*)malloc(sizeof(entity));
  p1->position = sf::Vector2f(50.f, (SCREEN_HEIGHT/2.f));
  p1->dimensions = sf::Vector2f(20.f,100.f);
  p1->type = entity_paddle;
  p1->component_flags =
    component_sprite|component_rigidbody|component_position|component_dimensions|component_colour|component_playercontrol;
  p1->rigidbody_shape = rigidbody_rectangle;
  p1->rigidbody_type = rigidbody_static;
  build_sprite(p1, &p1_sprite, &blank_texture);
  build_rigidbody(p1, &World, p1->rigidbody_type, p1->rigidbody_shape);
  p1->sprite->setColor(sf::Color::Yellow);
  entities.push_back(p1);

  // Create AI paddle
  sf::Sprite p2_sprite;
  entity* p2 = (entity*)malloc(sizeof(entity));
  p2->position = sf::Vector2f((SCREEN_WIDTH-50.f), (SCREEN_HEIGHT/2.f));
  p2->dimensions = sf::Vector2f(20.f,100.f);
  p2->type = entity_paddle;
  p2->component_flags =
    component_sprite|component_rigidbody|component_position|component_dimensions|component_colour;
  p2->rigidbody_shape = rigidbody_rectangle;
  p2->rigidbody_type = rigidbody_static;
  build_sprite(p2, &p2_sprite, &blank_texture);
  build_rigidbody(p2, &World, p2->rigidbody_type, p2->rigidbody_shape);
  p2->sprite->setColor((playmode == 0) ? sf::Color::Green : sf::Color::Red);
  entities.push_back(p2);


  // create ball
  sf::Sprite ball_sprite;
  entity* ball = (entity*)malloc(sizeof(entity));
  ball->position = sf::Vector2f((SCREEN_WIDTH/2),(SCREEN_HEIGHT/2));
  ball->dimensions = sf::Vector2f(50,50);
  ball->radius = 0.5f;
  ball->type = entity_ball;
  ball->component_flags = component_sprite|component_rigidbody|component_position;
  ball->rigidbody_shape = rigidbody_circle;
  ball->rigidbody_type = rigidbody_dynamic;
  ball->restitution = 1.f;
  build_sprite(ball, &ball_sprite, &ball_texture);
  ball->sprite->setScale(sf::Vector2f(ball->radius,ball->radius));
  build_rigidbody(ball, &World, ball->rigidbody_type, ball->rigidbody_shape);
  entities.push_back(ball);

  // Create divider
  sf::Sprite divider_sprite;
  entity* divider = (entity*)malloc(sizeof(entity));
  divider->position = sf::Vector2f((SCREEN_WIDTH/2)+5,0);
  divider->dimensions = sf::Vector2f(10,SCREEN_HEIGHT);
  divider->type = entity_static;
  divider->component_flags = component_sprite|component_position;
  build_sprite(divider, &divider_sprite, &blank_texture);
  divider->sprite->setOrigin(0,0);
  divider->sprite->setColor(sf::Color(200,200,200));

  // make sure that the divider is drawn below everything else
  auto it = entities.begin();
  it = entities.insert(it,divider);

  //create walls
  entity* wall0 = (entity*)malloc(sizeof(entity));
  wall0->position = sf::Vector2f((SCREEN_WIDTH/2),0);
  wall0->dimensions = sf::Vector2f(SCREEN_WIDTH,1);
  wall0->type = entity_static;
  wall0->rigidbody_shape = rigidbody_rectangle;
  wall0->rigidbody_type = rigidbody_static;
  wall0->component_flags = component_position|component_rigidbody|component_dimensions;
  build_rigidbody(wall0, &World, wall0->rigidbody_type, wall0->rigidbody_shape);
  entities.push_back(wall0);

  entity* wall1 = (entity*)malloc(sizeof(entity));
  wall1->position = sf::Vector2f((SCREEN_WIDTH/2),SCREEN_HEIGHT);
  wall1->dimensions = sf::Vector2f(SCREEN_WIDTH,1);
  wall1->type = entity_static;
  wall1->rigidbody_shape = rigidbody_rectangle;
  wall1->rigidbody_type = rigidbody_static;
  wall1->component_flags = component_position|component_rigidbody|component_dimensions;
  build_rigidbody(wall1, &World, wall1->rigidbody_type, wall1->rigidbody_shape);
  entities.push_back(wall1);

  ball->rigidbody->SetLinearVelocity(b2Vec2(-20.f,0.f));

  sf::Clock clk;
  sf::Time dt;
  while (window.isOpen())
  {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
      window.close();

    float mouse_y = sf::Mouse::getPosition(window).y;

    window.clear(sf::Color(110,110,110));
    for(entity* e : entities)
    {
      if(flag_is_set(e, component_playercontrol))
      {
        e->rigidbody->SetTransform(b2Vec2(e->rigidbody->GetPosition().x, mouse_y/SCALE), 0);
      }

      if(flag_is_set(e, component_rigidbody) && flag_is_set(e, component_sprite))
      {
        e->sprite->setPosition(
          e->rigidbody->GetPosition().x * SCALE,
          e->rigidbody->GetPosition().y * SCALE);

        e->sprite->setRotation(e->rigidbody->GetAngle());
      }

      if (flag_is_set(e, component_sprite))
      {
        window.draw(*(e)->sprite);
      }
    }
    window.display();
    World.Step(1.f/framerate, 8, 3);
    dt = clk.restart();
  }

  return(EXIT_SUCCESS);
}
/*
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
