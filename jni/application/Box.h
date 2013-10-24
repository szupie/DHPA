#ifndef BOX_H
#define BOX_H

#include <Zeni/Collision.h>
#include <Zeni/Model.h>
#include <Zeni/Quaternion.h>
#include <Zeni/Sound.h>
#include <Zeni/Vector3f.h>
#include "Thing.h"

using namespace Zeni;

class Box : public Thing {
public:
  Box(const Point3f &corner_ = Point3f(0.0f, 0.0f, 0.0f),
        const Vector3f &scale_ = Vector3f(1.0f, 1.0f, 1.0f),
        const Quaternion &rotation_ = Quaternion::Axis_Angle(Vector3f(0.0f, 0.0f, 1.0f), 0.0f));
  Box(const Box &rhs);
  Box & operator=(const Box &rhs);
  ~Box();

  void render();

  void collide();

  const Collision::Parallelepiped & get_body() const {return m_body;}

private:
  void create_body();

  // Level 1
  static Model * m_model;
  static unsigned long m_instance_count;

  Sound_Source * m_source;

  // Level 2
  Point3f m_corner;
  Vector3f m_scale;
  Quaternion m_rotation;

  // Level 3
  Collision::Parallelepiped m_body; // not motion so much as collision

  // Level 4
  // A stationary Crate has no controls
};

#endif
