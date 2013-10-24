#include <zenilib.h>

#include "DinoGameState.h"

using namespace Zeni;
using namespace Zeni::Collision;

int cameraDistance = 300;

DinoGameState::DinoGameState() { 
  m_player = new Player(Point3f(90.0f, 90.0f, 0.0f),
                        Vector3f(120.0f, 30.0f, 40.0f));
  Box * m_crate = new Box(Point3f(12.0f, 12.0f, 0.0f), Vector3f(30.0f, 30.0f, 30.0f));
  Box * crate2 = new Box(Point3f(50.0f, 12.0f, 0.0f),
                         Vector3f(30.0f, 30.0f, 30.0f));
  for (int i=0; i<70; i++) {
    Person * mook = new Person(Point3f(50.0f+rand()%(30+i*10), 50.0f+rand()%(30+i*10), 0.0f),
                                Vector3f(5.0f, 7.0f, 17.0f));    
    things.push_back(mook);
  }
  for (int i=0; i<5; i++) {
    Shooter * shooter = new Shooter(Point3f(50.0f+rand()%(30+i*10), 50.0f+rand()%(30+i*10), 0.0f),
                               Vector3f(5.0f, 7.0f, 17.0f));    
    things.push_back(shooter);
  }
  
  things.push_back(m_crate);
  things.push_back(crate2);
  
  m_camera = Camera(Point3f(0.0f, 0.0f, cameraDistance),
                    Quaternion(),
                    5.0f, 
                    10000.0f);
  m_camera.fov_rad = Global::pi / 3.0f;
  cameraYaw = 0;
  cameraPitch = 53.0f * Global::pi / 180.0f;
  
  set_pausable(true);
}

DinoGameState::~DinoGameState() {
  for (size_t i=0, length=things.size(); i<length; i++) {
    delete things[i];
    things[i] = NULL;
  }
  people.clear();
  things.clear();
}

void DinoGameState::on_push() {
  get_Window().set_mouse_state(Window::MOUSE_RELATIVE);
}

void DinoGameState::on_controller_button(const SDL_ControllerButtonEvent &event)	{
  if (event.type == SDL_CONTROLLERBUTTONDOWN) {
    switch (event.button) {
      case SDL_CONTROLLER_BUTTON_BACK:
        get_Game().push_state(new Popup_Menu_State);
        break;
        
      case SDL_CONTROLLER_BUTTON_A:
        m_player->run();
        break;
        
      default:
        break;
    }
  }
}

void DinoGameState::on_controller_axis(const SDL_ControllerAxisEvent &event) {
  int value = event.value;
  if (abs(value) < 10000) {
    value = 0;
  }
  switch(event.axis) {
    case SDL_CONTROLLER_AXIS_LEFTX:
      m_controls.lright = (value + 0.5f) / 32767.5f;
      break;
      
    case SDL_CONTROLLER_AXIS_LEFTY:
      m_controls.lup = (value + 0.5f) / 32767.5f;
      break;
      
    case SDL_CONTROLLER_AXIS_RIGHTX:
      m_controls.rright = (value + 0.5f) / 32767.5f;
      break;
      
    case SDL_CONTROLLER_AXIS_RIGHTY:
      m_controls.rup = (value + 0.5f) / 32767.5f;
      break;
      
    default:
      break;
  }
}

void DinoGameState::on_key(const SDL_KeyboardEvent &event) {
  switch(event.keysym.sym) {
    case SDLK_w:
      m_controls.forward = event.type == SDL_KEYDOWN;
      break;

    case SDLK_a:
      m_controls.left = event.type == SDL_KEYDOWN;
      break;

    case SDLK_s:
      m_controls.back = event.type == SDL_KEYDOWN;
      break;

    case SDLK_d:
      m_controls.right = event.type == SDL_KEYDOWN;
      break;

    case SDLK_LSHIFT:
      if(event.type == SDL_KEYDOWN) {
        m_player->run();
      }
      break;

    default:
      Gamestate_Base::on_key(event);
      break;
  }
}

void DinoGameState::on_mouse_motion(const SDL_MouseMotionEvent &event) {
  turnCamera(-event.xrel / 500.0f, -event.yrel / 500.0f);
}

void DinoGameState::turnCamera(const float &theta, const float &phi) {
  cameraYaw += theta;
  if (cameraPitch+phi > 0.1f && 
      cameraPitch+phi < Global::pi/2.0f) {
    cameraPitch += phi;
  }
  m_camera.position.x = m_player->getCenter().x+cameraDistance*cos(cameraYaw)*sin(cameraPitch);
  m_camera.position.y = m_player->getCenter().y+cameraDistance*sin(cameraYaw)*sin(cameraPitch);
  m_camera.position.z = m_player->getCenter().z+cameraDistance*cos(cameraPitch);
  
  for (size_t i=0, length=things.size(); i<length; i++) {
    while (things[i]->get_body().intersects(Parallelepiped(m_camera.position,
                                                           Vector3f(10,0,0),
                                                           Vector3f(0,10,0),
                                                           Vector3f(0,0,10)))) {
      m_camera.position = m_camera.position.interpolate_to(0.1f, m_player->getCenter());
    }
  }
}

void DinoGameState::perform_logic() {
  const Time_HQ current_time = get_Timer_HQ().get_time();
  float processing_time = float(current_time.get_seconds_since(time_passed));
  time_passed = current_time;
  
  people.clear();
  for (size_t i=0, length=things.size(); i<length; i++) {
    if (things[i] != NULL) {
      if (things[i]->removable) {
        delete things[i];
        things[i] = NULL;
        things.erase(things.begin()+i);
      } else if (things[i]->type & PERSON) {
        people.push_back((Person*)things[i]); 
      }
    }
  }

  /** Get forward and left vectors in the XY-plane **/
  const Vector3f forward = m_camera.get_forward().get_ij().normalized();
  const Vector3f left = m_camera.get_left().get_ij().normalized();

  /** Get velocity vector split into a number of axes **/
  float sagital = m_controls.forward - m_controls.back;
  float coronal = m_controls.left - m_controls.right;
  if (!sagital)
    sagital = -m_controls.lup;
  if (!coronal)
    coronal = -m_controls.lright;
  const Vector3f velocity = sagital * 50.0f * forward
                          + coronal * 50.0f * left
                          + m_player->runDirection;
  m_player->runDirection *= 0.95;
  const Vector3f x_vel = velocity.get_i();
  const Vector3f y_vel = velocity.get_j();
  
  if (sagital) {
    m_player->aimDirection(forward);
  }

  /** Bookkeeping for sound effects **/
  if(velocity.magnitude() != 0.0f)
    m_moved = true;
  
  boidPeople();
  for (size_t i=0, length=people.size(); i<length; i++) {
  //for (size_t length=people.size(), i=(rand()%length)/10; i<length; i+=length/10) {
    people[i]->hasLineOfSight = lineOfSight(people[i], m_player);
    people[i]->avoid(m_player);
    people[i]->perform_logic();
    //cout<<people[i]->m_velocity.x<<" and "<<people[i]->m_velocity.y<<endl;
  }

  /** Keep delays under control (if the program hangs for some time, we don't want to lose responsiveness) **/
  if(processing_time > 0.1f)
    processing_time = 0.1f;

  /** Physics processing loop**/
  for(float time_step = 0.05f;
      processing_time > 0.0f;
      processing_time -= time_step)
  {
    if(time_step > processing_time)
      time_step = processing_time;
    
    // move people
    for (size_t i=0, length=people.size(); i<length; i++) {
      const Vector3f personVelX = people[i]->m_velocity.get_i();
      const Vector3f personVelY = people[i]->m_velocity.get_j();
      partial_step_person(people[i], time_step, personVelX);
      partial_step_person(people[i], time_step, personVelY);
    }

    /** Try to move on each axis **/
    partial_step(time_step, x_vel);
    partial_step(time_step, y_vel);
  }
  
  // For running
  m_player->set_velocity(velocity);
  m_player->perform_logic();
  
  turnCamera(-m_controls.rright / 50.0f, -m_controls.rup / 50.0f);
  m_camera.look_at(m_player->getCenter());
}

void DinoGameState::partial_step_person(Person * person, const float &time_step, const Vector3f &velocity) {
  person->m_velocity = velocity;
  const Point3f backup_position = person->m_position;
  Parallelepiped backup_body = person->get_body();
  
  person->step(time_step);
  
  /** If collision with the crate has occurred, roll things back **/
  for (size_t i=0, length=things.size(); i<length; i++) {
    if (things[i] == person) continue;
    if (things[i]->get_body().intersects(person->get_body())) {
      person->m_position = backup_position;
    }
    if (things[i]->get_body().intersects(backup_body)) {
      // Stuck together
      person->m_position += Vector3f(backup_body.get_center()-things[i]->get_body().get_center()).get_ij().normalized();
    }
  }
}

void DinoGameState::partial_step(const float &time_step, const Vector3f &velocity) {
  m_player->set_velocity(velocity);
  const Point3f backup_position = m_player->getPosition();

  m_player->step(time_step);

  /** If collision has occurred, roll things back **/
  for (size_t i=0, length=things.size(); i<length; i++) {
    if (things[i]->get_body().intersects(m_player->get_body())) {
      if (things[i]->type & PERSON) {
        ((Person*)things[i])->crush();
      }
      if (m_moved)
      {
        get_Controllers().set_vibration(0, 1, 1);
        m_moved = false;
      }

      m_player->setPosition(backup_position);
    }
  }
}

bool DinoGameState::lineOfSight(Thing * a, Thing * b) {
  Collision::Line_Segment line = Collision::Line_Segment(a->get_body().get_center(), b->get_body().get_center());
  
  for (size_t i=0, length=things.size(); i<length; i++) {
    if (things[i] == a || things[i] == b) continue;
    if (line.intersects(things[i]->get_body())) {
      return false;
    }
  }
  return true;
}

void DinoGameState::boidPeople() {
  vector<Person *> quads[4];
  Point3f center;
  for (size_t i=0, length=people.size(); i<length; i++) {
    center += people[i]->m_position;
    if (people[i]->m_position.x > Person::boidCenter.x &&
        people[i]->m_position.y > Person::boidCenter.y) {
      quads[0].push_back(people[i]);
    }
    if (people[i]->m_position.x < Person::boidCenter.x &&
        people[i]->m_position.y > Person::boidCenter.y) {
      quads[1].push_back(people[i]);
    }
    if (people[i]->m_position.x < Person::boidCenter.x &&
        people[i]->m_position.y < Person::boidCenter.y) {
      quads[2].push_back(people[i]);
    }
    if (people[i]->m_position.x > Person::boidCenter.x &&
        people[i]->m_position.y < Person::boidCenter.y) {
      quads[3].push_back(people[i]);
    }
  }
  for (int quad=0; quad<4; quad++) {
    for (size_t i=0, ilength=quads[quad].size(); i<ilength; i++) {
      
      int antisocialness = rand()%50 + 1;
      // Cohesion to group center
      quads[quad][i]->m_velocity += Vector3f(Person::boidCenter-(quads[quad][i]->getCenter()+quads[quad][i]->m_velocity))
      *((int(!quads[quad][i]->scared)+0.02f)/10.0f/antisocialness);
      
      for (size_t j=0, jlength=quads[quad].size(); j<jlength; j++) {
        // Avoidance
        if (quads[quad][i] == quads[quad][j]) continue;
        float distance = quads[quad][i]->get_body().shortest_distance(quads[quad][j]->get_body());
        if (distance < antisocialness) {
          quads[quad][i]->m_velocity += Vector3f(quads[quad][i]->getCenter()-quads[quad][j]->getCenter()).normalized()
          *(int(!quads[quad][i]->scared)*antisocialness/(distance+1));
        }
      }
    } 
  }
  Person::boidCenter = Vector3f(center)/float(people.size());
  /*Point3f center;
  for (size_t i=0, ilength=people.size(); i<ilength; i++) {
    center += people[i]->m_position;
    for (size_t j=0, jlength=people.size(); j<jlength; j++) {
      int antisocialness = rand()%50 + 1;
      // Group center
      if (i+1 == ilength) {
        people[j]->m_velocity += Vector3f(Vector3f(center)/float(ilength)-people[j]->getCenter())
        *((int(!people[j]->scared)+0.02f)/30.0f/antisocialness);
      }
      // Avoidance
      if (people[i] == people[j]) continue;
      float distance = people[i]->get_body().shortest_distance(people[j]->get_body());
      if (distance < antisocialness) {
        people[j]->m_velocity += Vector3f(people[j]->getCenter()-people[i]->getCenter()).normalized()
        *(int(!people[j]->scared)*antisocialness/(distance+1));
      }
    }
  }*/
}

void DinoGameState::render() {
  get_Video().set_backface_culling(true);
  
  get_Video().set_lighting(true);
  get_Video().set_ambient_lighting(Color(0.01f, 0.3f, 0.3f, 0.3f));
  Light skyLight;
  skyLight.position = Point3f(12.0f, 12.0f, 500.0f);
  skyLight.diffuse = Color(0.0f, 1.0f, 1.0f, 1.0f);
  skyLight.specular = Color(0.8f, 1.0f, 1.0f, 1.0f);
  get_Video().set_Light(0, skyLight);
  
  get_Video().set_3d(m_camera);
  
  for (size_t i=0, length=things.size(); i<length; i++) {
    things[i]->render();
  }
  m_player->render();
  
  get_Video().set_lighting(false);
  
  get_Video().set_2d(make_pair(Point2f(0.0f, 0.0f), Point2f(1280.0f, 800.0f)), true);
  get_Fonts()["title"].render_text("FPS: " + ulltoa(get_Game().get_fps()), Point2f(0.0f, 0.0f), Color());
  get_Fonts()["title"].render_text("People count: " + ulltoa(people.size()), Point2f(0.0f, 100.0f), Color());
}
