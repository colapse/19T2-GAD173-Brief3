#include "pch.h"
#include "MovableObject.h"

MovableObject::MovableObject()
{
	playerMovement = sf::Vector2f(0, 0);
}


MovableObject::~MovableObject()
{
}

void MovableObject::Update() {
	SetPosition(sf::Vector2f(this->pos.x -1, this->pos.x -1));
	
}

void MovableObject::OnKeyUp(sf::Keyboard::Key key) {
	// TODO
}

void MovableObject::OnKeyDown(sf::Keyboard::Key key) {
	// TODO
}

void MovableObject::OnCollisionEnter(std::shared_ptr<Collision> collider) {
	// TODO
}