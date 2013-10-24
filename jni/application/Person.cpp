#include <zenilib.h>

#include "Person.h"

using namespace Zeni;
using namespace Zeni::Collision;

Person::Person(const Point3f &position,
               const Vector3f &scale) {
  type |= PERSON;
  
  m_position = position;
  m_scale = scale;
  m_velocity = Vector3f();
  
  speed = 1000;
  scaredness = 100;
  
  if(!m_instance_count)
    m_model = new Model("models/box.3ds");
  ++m_instance_count;

  create_body();
}

Person::Person(const Person &rhs) {
  m_position = rhs.m_position;
  m_scale = rhs.m_scale;
  ++m_instance_count;

  create_body();
}

Person & Person::operator=(const Person &rhs) {
  m_position = rhs.m_position;
  m_scale = rhs.m_scale;

  create_body();

  return *this;
}

Person::~Person() {
  if(!--m_instance_count) {
    delete m_model;
    m_model = 0lu;
  }
}

Point3f Person::getCenter() {
  return m_position+(m_scale*0.5f);
}

void Person::render() {
  const std::pair<Vector3f, float> rotation = m_rotation.get_rotation();

  m_model->set_translate(m_position);
  m_model->set_scale(m_scale);
  m_model->set_rotate(rotation.second, rotation.first);

  m_model->render();
}

void Person::crush() {
  removable = true;
}

void Person::collide() {
}

void Person::avoid(Thing * target) {
  float distance = get_body().shortest_distance(target->get_body());
  int safeDistance = rand()%scaredness + rand()%scaredness;
  scared = false;
  if (distance < safeDistance) scared = true;
  if (scared) {
    if (hasLineOfSight) {
      m_velocity += Vector3f(getCenter()-target->get_body().get_center()).get_ij().normalized()*(speed/(1+distance));
    }
    // Random moves
    Vector3f perpendicular = m_velocity%Vector3f(0,0,1);
    m_velocity += perpendicular * float(rand()%100-50)/50.0f;
    m_velocity += Vector3f(float(rand()%100-50)/10.0f, float(rand()%100-50)/10.0f, 0);
  }
}

void Person::perform_logic() {
  m_velocity = m_velocity*0.9f;
  // Round out movement
  m_velocity.x = m_velocity.x*0.9f+m_velocity.y*0.1f;
  m_velocity.y = m_velocity.y*0.9f+m_velocity.x*0.1f;
  
  // Enforce top speed
  if (m_velocity.magnitude() > 50) {
    m_velocity = m_velocity.normalize()*50;
  }
}

void Person::step(const float &time_step) {
  m_position += time_step * m_velocity;
  create_body();
}

void Person::create_body() {
  m_body = Parallelepiped(m_position,
                          m_rotation * m_scale.get_i(),
                          m_rotation * m_scale.get_j(),
                          m_rotation * m_scale.get_k());
}

Model * Person::m_model = 0;
unsigned long Person::m_instance_count = 0lu;
Point3f Person::boidCenter = Point3f();

