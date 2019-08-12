#include "pch.h"
#include "PlayerObject.h"

#ifndef Level
#include "Level.h"
#endif // !Level

#ifndef _IOSTREAM_
#include <iostream>
#endif // !_IOSTREAM_


PlayerObject::PlayerObject(float width, float height)
{
	collider = sf::RectangleShape(sf::Vector2f(width, height));
}

PlayerObject::PlayerObject()
{
	collider = sf::RectangleShape(sf::Vector2f(32, 32)); // TODO make dynamic tile size
}


PlayerObject::~PlayerObject()
{
}

void PlayerObject::SetPosition(sf::Vector2f pos) {
	this->pos = pos;
	collider.setPosition(pos);
	sprite.setPosition(pos);
	playerCamera->setCenter(pos);
}

void PlayerObject::Start() {
	collider.setFillColor(sf::Color::Green); // For Debug
}

void PlayerObject::Update() {
	DoCollisionCheck();
	grounded = IsGrounded();
	
	CalculateMovement();

	Move(objectMovement * Level::deltaTime);

	//Cooldown
	if (jumpCooldown > 0) {
		jumpCooldown -= Level::deltaTime;
	}
}

void PlayerObject::OnKeyUp(sf::Keyboard::Key key) {
	switch (key) {
	case sf::Keyboard::Key::Left:
	case sf::Keyboard::Key::A:
		inputLeft = false;
		break;
	case sf::Keyboard::Key::Right:
	case sf::Keyboard::Key::D:
		inputRight = false;
		break;
	case sf::Keyboard::Key::Space:
		inputJump = false;
		break;
	}
}

void PlayerObject::OnKeyDown(sf::Keyboard::Key key) {
	switch (key) {
	case sf::Keyboard::Key::Left:
	case sf::Keyboard::Key::A:
		inputLeft = true;
		break;
	case sf::Keyboard::Key::Right:
	case sf::Keyboard::Key::D:
		inputRight = true;
		break;
	case sf::Keyboard::Key::Space:
		inputJump = true;
		break;
	}
}

void PlayerObject::Move(sf::Vector2f movementVector) {
	collider.move(movementVector);
	sprite.move(movementVector);
	playerCamera->move(movementVector);

	this->pos = collider.getPosition();
}

void PlayerObject::OnTriggerEnter(std::shared_ptr<Collision> collider) {
	if (collider->colliderObject->prefab->gameObjectId == "Coin") {
		coins++;
		collider->colliderObject->Destroy();
		std::cout << "GOT A COIN!" << std::endl;
		return;
	}
}