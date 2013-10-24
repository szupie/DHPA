#include "Shooter.h"

Shooter::Shooter(const Point3f &position,
                 const Vector3f &scale) : Person(position, scale) {
  type |= SHOOTER;
}
