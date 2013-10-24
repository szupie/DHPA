#ifndef PLAYER_H
#define PLAYER_H

#include <zenilib.h>
#include "Thing.h"

using namespace Zeni;
using namespace std;

class Player : public Thing {
public:
  Player(const Point3f position,
         const Vector3f scale);
  ~Player();

  // Level 2
  void setPosition(const Point3f &position);
  Point3f getPosition();
  Point3f getCenter();

  // Level 3
  const Collision::Parallelepiped & get_body() const {return m_body;}

  const Vector3f & get_velocity() const {return m_velocity;}
  void set_velocity(const Vector3f &velocity_) {m_velocity = velocity_;}

  void run();
  Vector3f runDirection;
  float runExhaustion;
  
  void aimDirection(Vector3f direction);

  void step(const float &time_step);
  
  void perform_logic();
  void render();
  
  Quaternion m_rotation;

private:
  void create_body();
  
  Model * m_model;
  
  Point3f m_position;

  // Level 2
  Vector3f m_scale;
  Quaternion goalYaw;

  // Level 3
  Collision::Parallelepiped m_body; // collision
  Vector3f m_velocity;

  // Level 4
  // Controls are external to Player
};

#endif
