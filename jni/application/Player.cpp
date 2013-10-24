#include <zenilib.h>

#include "Player.h"

using namespace Zeni;
using namespace Zeni::Collision;

Player::Player(const Point3f position,
               const Vector3f scale) {
  type |= PLAYER;
  
  m_position = position;
  m_scale = scale;
  m_model = new Model("models/box.3ds");
  
  runExhaustion = 0.0f;
  runDirection = Vector3f();
  create_body();
}

Player::~Player() {
  delete m_model;
  m_model = 0lu;
}

// Level 2
void Player::setPosition(const Point3f &position) {
  m_position = position;
  create_body();
}

Point3f Player::getPosition() {
  return m_position;
}

Point3f Player::getCenter() {
  return m_position+(m_scale*0.5f);
}

void Player::run() {
  if(runExhaustion < 100) {
    runDirection = m_velocity*5;
    runExhaustion = runDirection.magnitude();
  }
}

void Player::aimDirection(Vector3f direction) {
  //goalYaw = rotation;
}

void Player::step(const float &time_step) {
  //m_rotation = m_rotation*0.2f+goalRotation*0.8f;
  m_position += time_step * m_velocity;
  create_body();
}

void Player::perform_logic() {
  runExhaustion += m_velocity.magnitude();
  if (runExhaustion > 0) runExhaustion -= 60;
}

void Player::render() {
  const pair<Vector3f, float> rotation = m_rotation.get_rotation();
  //cout<<rotation.second<<" and vector is "<<rotation.first.x<<", "<<rotation.first.y<<", "<<rotation.first.z<<endl;
  
  m_model->set_translate(m_position);
  m_model->set_scale(m_scale);
  m_model->set_rotate(rotation.second, rotation.first);
  
  m_model->render();
}

void Player::create_body() {

  m_body = Parallelepiped(m_position,
                          m_rotation * m_scale.get_i(),
                          m_rotation * m_scale.get_j(),
                          m_rotation * m_scale.get_k());
}

