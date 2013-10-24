#ifndef THING_H
#define THING_H

#include <zenilib.h>

enum ThingType { THING=1, BOX=1<<1, PLAYER=1<<2, PERSON=1<<3, SHOOTER=1<<4 };

class Thing {
public:
  Thing() {
    type = THING;
    removable = false;
  };

  virtual void render() = 0;

  virtual const Zeni::Collision::Parallelepiped & get_body() const = 0;
  
  int type;
  bool removable;
};

#endif
