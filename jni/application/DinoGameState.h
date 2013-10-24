#ifndef DINOGAMESTATE_H
#define DINOGAMESTATE_H

#include "Crate.h"
#include "Box.h"
#include "Player.h"
#include "Person.h"
#include "Shooter.h"
#include <zenilib.h>
#include <vector>

using namespace std;

class DinoGameState : public Zeni::Gamestate_Base {
  struct Controls {
    Controls() : forward(false), left(false), back(false), right(false),
    lright(0.0f), lup(0.0f), rright(0.0f), rup(0.0f) {}

    bool forward;
    bool left;
    bool back;
    bool right;
    float lright;
    float lup;
    float rright;
    float rup;
  } m_controls;

public:
  DinoGameState();
  ~DinoGameState();

  void on_push();
  
  void on_controller_button(const SDL_ControllerButtonEvent &event);
  void on_controller_axis(const SDL_ControllerAxisEvent &event);
  void on_key(const SDL_KeyboardEvent &event);
  void on_mouse_motion(const SDL_MouseMotionEvent &event);
  
  void turnCamera(const float &theta, const float &phi);

  void perform_logic();

  void render();

private:
  void partial_step(const float &time_step, const Zeni::Vector3f &velocity);
  void partial_step_person(Person * person, const float &time_step, const Zeni::Vector3f &velocity);
  
  bool lineOfSight(Thing * a, Thing * b);
  void boidPeople();

  Zeni::Time_HQ time_passed;

  Player * m_player;
  vector<Thing *> things;
  vector<Person *> people;

  bool m_moved;
  
  Camera m_camera;
  float cameraYaw;
  float cameraPitch;
};

#endif
