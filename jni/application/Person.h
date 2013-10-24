#ifndef PERSON_H
#define PERSON_H

#include <zenilib.h>
#include "Thing.h"

using namespace Zeni;
using namespace std;

class Person : public Thing {
public:
  Person(const Point3f &position = Point3f(0.0f, 0.0f, 0.0f),
        const Vector3f &scale = Vector3f(1.0f, 1.0f, 1.0f));
  Person(const Person &rhs);
  Person & operator=(const Person &rhs);
  ~Person();
  
  Point3f getCenter();

  void render();

  void collide();
  
  void avoid(Thing * target);
  void perform_logic();
  void step(const float &time_step);

  const Collision::Parallelepiped & get_body() const {return m_body;}
  
  void crush();
  
  Point3f m_position;
  Vector3f m_velocity;
  bool hasLineOfSight;
  bool scared;
  
  static Point3f boidCenter;
  
protected:
  int speed;
  int scaredness;

private:
  void create_body();

  // Level 1
  static Model * m_model;
  static unsigned long m_instance_count;

  // Level 2
  Vector3f m_scale;
  Zeni::Quaternion m_rotation;

  // Level 3
  Collision::Parallelepiped m_body; // not motion so much as collision

  // Level 4
  // A stationary Crate has no controls
};

#endif
