#ifndef SYSTEMS_HPP
#define SYSTEMS_HPP

#include "entity.hpp"
#include "util.hpp"

inline void
AI_system(Mem *world, unsigned int entity, int hidden,
          matrix Theta1, matrix Theta2, std::vector<float> inputs, float damping)
{
    vector a1(inputs.size()+1);
    a1(0) = 1;
    a1(1) = inputs[0];
    a1(2) = inputs[1];
    a1(3) = inputs[2];
    a1(4) = inputs[3];

    vector z2 = prod(a1,Theta1);

    vector tmp = sigmoid(z2);
    vector a2(hidden+1);

    a2(0) = 1;
    for (unsigned long i=1; i < a2.size(); ++i)
    {
      a2(i) = tmp(i-1);
    }

    vector h = sigmoid(prod(a2, Theta2));

    float target = (h(0) * SCREEN_HEIGHT/SCALE);
    world->agent[entity].target_y =
      target - (target - world->rigidbody[entity].rigidbody->GetPosition().y) * damping;
}

inline void
physics_system(Mem *world, unsigned int entity)
{
    world->transform[entity].position =
      sf::Vector2f(
          world->rigidbody[entity].rigidbody->GetPosition().x*SCALE,
          world->rigidbody[entity].rigidbody->GetPosition().y*SCALE);

    world->sprite[entity].sprite.setRotation(world->rigidbody[entity].rigidbody->GetAngle() * (180 / PI));
}

inline void
text_rendering_system(Mem *world, unsigned int entity, sf::RenderWindow *win)
{
    unsigned int agent = world->text[entity].agent;
    world->text[entity].text.setString(std::to_string(world->agent[agent].score));
    win->draw(world->text[entity].text);
}

inline void
sprite_rendering_system(Mem *world, unsigned int entity, sf::RenderWindow *win)
{
    world->sprite[entity].sprite.setPosition(world->transform[entity].position);
    sf::Sprite s = world->sprite[entity].sprite;
    win->draw(s);
}

inline void
paddle_move_system(Mem *world, unsigned int entity, float mouse_y, float damping = 0.f)
{
  b2Vec2 pos = world->rigidbody[entity].rigidbody->GetPosition();
  if (mouse_y >= world->transform[entity].dimensions.y / 2
      && mouse_y <= SCREEN_HEIGHT - (world->transform[entity].dimensions.y /2))
  {
     world->agent[entity].target_y = mouse_y / SCALE;
  }
  pos.y = world->agent[entity].target_y - (world->agent[entity].target_y - pos.y) * damping;
  world->rigidbody[entity].rigidbody->SetTransform(pos, 0);
}

inline void
ball_correction_system(Mem *world, unsigned int entity)
{
    float vert_velocity = world->rigidbody[entity].rigidbody->GetLinearVelocity().y;
    if (std::abs(vert_velocity) < 3.f)
    {
      float new_vert_velocity;
      new_vert_velocity = (vert_velocity < 0) ? -2.f : 2.f;
      b2Vec2 ball_vertical_velocity = b2Vec2(0.f, new_vert_velocity);
      world->rigidbody[entity].rigidbody->ApplyForceToCenter(ball_vertical_velocity, true);
    }

    float horiz_velocity = world->rigidbody[entity].rigidbody->GetLinearVelocity().x;
    if (std::abs(horiz_velocity) < world->rigidbody[entity].speed)
    {
      float new_horiz_velocity;
      new_horiz_velocity = (horiz_velocity < 0) ? -5.f : 5.f;
      b2Vec2 ball_horizical_velocity = b2Vec2(new_horiz_velocity, 0.f);
      world->rigidbody[entity].rigidbody->ApplyForceToCenter(ball_horizical_velocity, true);
    }
}

inline void
data_collect_system(Mem *world, unsigned int entity,
  float dist, float ball_y, b2Vec2 ball_v, std::ofstream &human_file)
{
    float target_y = (world->agent[entity].target_y / (SCREEN_HEIGHT/SCALE)) * 100.f;

    std::ostringstream buff;
    buff << dist << DELIM << ball_y << DELIM << ball_v.x << DELIM << ball_v.y << DELIM << target_y << "\n";
    human_file << buff.str();
}

inline void
scoring_system(Mem *world, unsigned int p1, unsigned int p2, unsigned int ball)
{
  if (world->rigidbody[ball].rigidbody->GetPosition().x < 0
        || world->rigidbody[ball].rigidbody->GetPosition().x > (SCREEN_WIDTH / SCALE))
    {
      if (world->rigidbody[ball].rigidbody->GetPosition().x < 0)
        world->agent[p2].score += 1;
      else
        world->agent[p1].score += 1;

      float choices[2] = {-1.f, 1.f};
      std::srand(time(nullptr));
      int r = rand()%2;
      float c = choices[r];
      world->rigidbody[ball].rigidbody->SetTransform(
        b2Vec2((SCREEN_WIDTH/2)/SCALE, (SCREEN_HEIGHT/2)/SCALE), 0);

      float speed = world->rigidbody[ball].speed;
      world->rigidbody[ball].rigidbody->SetLinearVelocity(
        b2Vec2(speed * c, random_float(-speed, speed)));
    }
}

#endif
