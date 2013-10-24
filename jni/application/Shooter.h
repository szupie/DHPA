#ifndef SHOOTER_H
#define SHOOTER_H

#include "Person.h"

class Shooter : public Person {
public:
  Shooter(const Point3f &position = Point3f(0.0f, 0.0f, 0.0f),
        const Vector3f &scale = Vector3f(1.0f, 1.0f, 1.0f));

};

#endif
