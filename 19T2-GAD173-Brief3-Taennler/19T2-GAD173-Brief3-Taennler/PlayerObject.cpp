#include "pch.h"
#include "PlayerObject.h"

#ifndef Level
#include "Level.h"
#endif // !Level

#ifndef EnemyObject
#include "EnemyObject.h"
#endif // !EnemyObject

#ifndef _IOSTREAM_
#include <iostream>
#endif // !_IOSTREAM_

#ifndef _CMATH_
#include <math.h>
#endif // !_CMATH_

PlayerObject::PlayerObject(float width, float height)
{
	sprite = new sf::Sprite;
	collider = sf::RectangleShape(sf::Vector2f(width, height));
}

PlayerObject::PlayerObject()
{
	sprite = new sf::Sprite;
	collider = sf::RectangleShape(sf::Vector2f(32, 32)); // TODO make dynamic tile size
}


PlayerObject::~PlayerObject()
{
}

void PlayerObject::SetPosition(sf::Vector2f pos) {
	this->pos = pos;
	collider.setPosition(pos);
	if(sprite != nullptr)
		sprite->setPosition(pos);
	if(playerCamera != nullptr)
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

	// Check for enemy/exit/lava collision
	if (collisions.size() > 0) {
		for(std::shared_ptr<Collision> collision : collisions) {
			if (isAlive && collision->colliderObject->tag == "Enemy" && typeid(*collision->colliderObject) == typeid(EnemyObject)) {
				std::shared_ptr<EnemyObject> enemyObj = std::dynamic_pointer_cast<EnemyObject>(collision->colliderObject);
				if (enemyObj->isAlive) {
					std::cout << "TODO: Player Died through enemy!" << std::endl;
					isAlive = false;
					blockMovement = true;
				}
			}
		}
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
	case sf::Keyboard::Key::Enter:
		if(isAlive)
			ThrowTrap();
		break;
	}
}

void PlayerObject::OnKeyDown(sf::Keyboard::Key key) {
	switch (key) {
	case sf::Keyboard::Key::Left:
	case sf::Keyboard::Key::A:
		inputLeft = true;
		lookDirection.x = -1;
		break;
	case sf::Keyboard::Key::Right:
	case sf::Keyboard::Key::D:
		inputRight = true;
		lookDirection.x = 1;
		break;
	case sf::Keyboard::Key::Space:
		inputJump = true;
		break;
	}
}

void PlayerObject::Move(sf::Vector2f movementVector) {
	movementVector = CalculateFinalMovementVector(movementVector);
	collider.move(movementVector);
	sprite->move(movementVector);
	playerCamera->move(movementVector);

	this->pos = collider.getPosition();
}

void PlayerObject::OnTriggerEnter(std::shared_ptr<Collision> collider) {
	if (isAlive && collider->colliderObject->tag == "Coin") {
		AddCoins(1);
		collider->colliderObject->Destroy();
		return;
	}
	else if (isAlive && collider->colliderObject->tag == "2") {
		sf::Vector2f dis = Level::GetObjectDistanceWithinAreaVector(this->collider.getGlobalBounds(), collider->colliderObject->collider.getGlobalBounds());

		if (dis.x > 3 || dis.x < -3 || dis.y > 3 || dis.y < -3) {
			std::cout << "TODO: Player died through Lava!" << std::endl;
			isAlive = false;
			blockMovement = true;
		}
	}
	else if (isAlive && collider->colliderObject->tag == "Exit") {
		std::cout << "TODO: Player reached the exit!" << std::endl;
		blockMovement = true;
		Level::instance->playerReachedEnd = true;
	}
}

void PlayerObject::AddCoins(int amount) {
	coins += amount;

	if (OnCoinAmountChange.size() > 0) {
		for (std::function<void(int)> func : OnCoinAmountChange) {
			func(coins);
		}
	}
}

int PlayerObject::GetCoinCount() {
	return coins;
}

void PlayerObject::AddPoints(int amount) {
	score += amount;

	if (OnScoreChange.size() > 0) {
		for (std::function<void(int)> func : OnScoreChange) {
			func(score);
		}
	}
}

int PlayerObject::GetScore() {
	return score;
}

void PlayerObject::ThrowTrap() {
	if (Level::instance == nullptr)
		return;

	std::shared_ptr<MovableObject> trapObj = std::make_shared<MovableObject>();
	
	trapObj->prefab = GameObjectPrefab::gameObjectPrefabs["Trap"];
	trapObj->sprite->setTexture(*GameObjectPrefab::gameObjectPrefabTextures["Trap"]);

	sf::Vector2f spawnPos((pos.x + (int)ceil(lookDirection.x)*32), pos.y - 16);
	trapObj->SetPosition(spawnPos);
	trapObj->isSolid = GameObjectPrefab::gameObjectPrefabs["Trap"]->isSolid;
	trapObj->tag = "Trap";
	trapObj->renderLayer = 3;

	Level::instance->AddGameObject(trapObj);
	
	sf::Vector2f force = sf::Vector2f(80,-.5*gravity);
	force.x *= lookDirection.x;
	force.y *= (lookDirection.y==0?1: lookDirection.y);

	trapObj->AddForce(force);
}

void PlayerObject::Destroy() {
	prefab = nullptr;
	if (collisions.size() > 0) {
		for (std::shared_ptr<Collision> col : collisions) {
			col.reset();
			col = nullptr;
		}
	}
	collisions.clear();

	playerCamera.reset();
	playerCamera = nullptr;

	for (std::function<void()> func : OnRequestDestroy) {
		func();
	}
}