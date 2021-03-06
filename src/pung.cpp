#include "pung.hpp"
#include "world.hpp"
#include "entity.hpp"
#include "components.hpp"
#include "systems.hpp"
#include "util.hpp"

int
main (int argc, char ** argv)
{
  std::ofstream human_file;
  std::ifstream ai_file;

  std::string filepath;
  std::string filename;
  int32 playmode;
  int32 hidden_nodes;

  real32 framerate = 60.f;
  int32  inputs = 5;

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

    TCLAP::ValueArg<int32> playmodeArg(
      "p",
      "playmode",
      "Play mode",
      true,
      0,
      "int");

    TCLAP::ValueArg<int32> hiddenNodesArg(
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

  int32 weights_length = std::count (std::istreambuf_iterator<char>(ai_file),
                                   std::istreambuf_iterator<char>(), '\n');
  ai_file.clear();
  ai_file.seekg(0, std::ios::beg);

  vector Weights(weights_length);
  matrix Theta1(inputs,hidden_nodes);
  matrix Theta2(hidden_nodes+1,1);
  vector h1(hidden_nodes+1);

  std::string line;
  size_t index = 0;
  while(getline(ai_file, line))
  {
    real32 w = std::stof(line);
    Weights(index) = w;
    ++index;
  }
  ai_file.close();

  std::vector<matrix> Theta;
  if (playmode == 1)
  {
    Theta = split_theta(Weights, inputs, hidden_nodes);
    Theta1 = Theta[0];
    Theta2 = Theta[1];
  }

  sf::Texture blank_texture;
  blank_texture.loadFromFile("assets/images/blank.png");

  sf::Texture ball_texture;
  ball_texture.loadFromFile("assets/images/ball.png");

  sf::Font score_font;
  score_font.loadFromFile("assets/fonts/Precursive.otf");

  // Do window stuff
  sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32), "PUNG");
  window.setMouseCursorVisible(false);
  window.setFramerateLimit(60);

  b2Vec2 Gravity(0.f, 0.f);
  b2World PhysicsWorld(Gravity);

  Mem *world = createWorld();
  Assert(world);

  uint32 divider = createEntity(world);
  world->type[divider].type = entity_static;
  add_sprite(world, divider, &blank_texture,
             sf::Vector2f(SCREEN_WIDTH/2,SCREEN_HEIGHT/2),
             sf::Vector2f(10,SCREEN_HEIGHT));
  world->sprite[divider].sprite.setColor(sf::Color(200,200,200));

  uint32 ball = createEntity(world);
  world->type[ball].type = entity_ball;
  add_sprite(world, ball, &ball_texture,
             sf::Vector2f(SCREEN_WIDTH/2,SCREEN_HEIGHT/2),
             sf::Vector2f(50,50));
  world->rigidbody[ball].radius = 0.5f;
  world->rigidbody[ball].speed = 20.f;
  add_rigidbody(world, ball, &PhysicsWorld, rigidbody_dynamic, rigidbody_circle);
  world->sprite[ball].sprite.setScale(sf::Vector2f(0.75f,0.75f));

  uint32 p1 = createEntity(world, component_playercontrol);
  world->type[p1].type = entity_paddle;
  add_sprite(world, p1, &blank_texture,
             sf::Vector2f(50,SCREEN_HEIGHT/2),
             sf::Vector2f(20,100));
  world->sprite[p1].sprite.setColor(sf::Color::Yellow);
  add_rigidbody(world, p1, &PhysicsWorld, rigidbody_static, rigidbody_rectangle);
  add_agent(world, p1);

  uint32 p2 = createEntity(world);
  int32 flags = (playmode) ? component_nncontrol : component_simpleai;
  set_flags(world, p2, flags);
  world->type[p2].type = entity_paddle;
  add_sprite(world, p2, &blank_texture,
             sf::Vector2f(SCREEN_WIDTH-50,SCREEN_HEIGHT/2),
             sf::Vector2f(20,100));
  add_rigidbody(world, p2, &PhysicsWorld, rigidbody_static, rigidbody_rectangle);
  add_agent(world, p2);
  (playmode == 0)
    ? world->sprite[p2].sprite.setColor(sf::Color::Green)
    : world->sprite[p2].sprite.setColor(sf::Color::Red);

  uint32 p1_score = createEntity(world, component_text);
  add_text(world, p1_score, p1, &score_font, 32, sf::Vector2f((SCREEN_WIDTH/4),10));

  uint32 p2_score = createEntity(world, component_text);
  add_text(world, p2_score, p2, &score_font, 32, sf::Vector2f(SCREEN_WIDTH-(SCREEN_WIDTH/4),10));

  uint32 wall0 = createEntity(world);
  add_rigidbody(world, wall0, &PhysicsWorld, rigidbody_static, rigidbody_rectangle,
    sf::Vector2f((SCREEN_WIDTH/2),0), sf::Vector2f(SCREEN_WIDTH,1));

  uint32 wall1 = createEntity(world);
  add_rigidbody(world, wall1, &PhysicsWorld, rigidbody_static, rigidbody_rectangle,
    sf::Vector2f((SCREEN_WIDTH/2),SCREEN_HEIGHT), sf::Vector2f(SCREEN_WIDTH,1));

  world->rigidbody[ball].rigidbody->SetLinearVelocity(b2Vec2(world->rigidbody[ball].speed, 0.f));

  real32 damping = 0.4f;


  sf::Clock clk;
  sf::Time dt;
  while (window.isOpen())
  {
    sf::Event event;
    while(window.pollEvent(event))
      if (event.type == sf::Event::Closed)
        window.close();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
      window.close();

    real32 dist;
    b2Vec2 ball_v = world->rigidbody[ball].rigidbody->GetLinearVelocity();
    real32 ball_y = ((world->rigidbody[ball].rigidbody->GetPosition().y) / (SCREEN_HEIGHT/SCALE)) * 100;
    real32 mouse_y = sf::Mouse::getPosition(window).y;

    scoring_system(world, p1, p2, ball);

    window.clear(sf::Color(110,110,110));

    for(int32 entity=0; entity<ENTITY_COUNT; ++entity)
    {
      if(world->type[entity].type == entity_null)
        break;

      if(flag_is_set(world, entity, component_sprite) && flag_is_set(world, entity, component_rigidbody))
      {
        if (flag_is_set(world, entity, component_nncontrol))
        {
          dist = std::abs(
            world->rigidbody[ball].rigidbody->GetPosition().x
            - world->rigidbody[entity].rigidbody->GetPosition().x);
          std::vector<real32> inputs = { dist, ball_y, ball_v.x, ball_v.y };
          AI_system(world, entity, hidden_nodes, Theta1, Theta2, inputs, damping);
        }

        if (flag_is_set(world, entity, component_simpleai))
        {
          dist = std::abs(
            world->rigidbody[ball].rigidbody->GetPosition().x
            - world->rigidbody[entity].rigidbody->GetPosition().x);
          data_collect_system(world, p1, dist, ball_y, ball_v, human_file);

          world->agent[entity].target_y = world->rigidbody[ball].rigidbody->GetPosition().y;
        }

        if(flag_is_set(world, entity, component_playercontrol))
          paddle_move_system(world, entity, mouse_y, damping);

        if(!flag_is_set(world, entity, component_playercontrol) && world->type[entity].type == entity_paddle)
          paddle_move_system(world, entity, world->agent[entity].target_y, damping);

        if(world->type[entity].type == entity_ball)
          ball_correction_system(world, entity);

        physics_system(world, entity);
      }

      if (flag_is_set(world, entity, component_text))
        text_rendering_system(world, entity, &window);

      if(flag_is_set(world, entity, component_sprite))
        sprite_rendering_system(world, entity, &window);
    }

    window.display();
    PhysicsWorld.Step(1.f/framerate, 8, 3);
    dt = clk.restart();

  }

  return(EXIT_SUCCESS);
}

