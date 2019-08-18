#include "pch.h"
#include "MovableObject.h"

#ifndef Level
#include "Level.h"
#endif // !Level

#ifndef _IOSTREAM_
#include <iostream>
#endif // !_IOSTREAM_

#ifndef _CMATH_
#include <math.h>
#endif // !_CMATH_


MovableObject::MovableObject(float width, float height)
{
	sprite = new sf::Sprite;
	collider = sf::RectangleShape(sf::Vector2f(width, height));
	if(isSolid)
		collider.setFillColor(sf::Color::Yellow); // For Debug
}

MovableObject::MovableObject()
{
	sprite = new sf::Sprite;
	collider = sf::RectangleShape(sf::Vector2f(32, 32)); // TODO make dynamic tile size
	collider.setFillColor(sf::Color::Yellow); // For Debug
	objectMovement = sf::Vector2f(0, 0);
}


MovableObject::~MovableObject()
{
}

void MovableObject::Update() {
	DoCollisionCheck();
	grounded = IsGrounded();

	CalculateMovement();

	if(!blockMovement)
		Move(objectMovement * Level::deltaTime);

	//Cooldown
	if (jumpCooldown > 0) {
		jumpCooldown -= Level::deltaTime;
	}
}

void MovableObject::OnKeyUp(sf::Keyboard::Key key) {
	// TODO
}

void MovableObject::OnKeyDown(sf::Keyboard::Key key) {
	// TODO
}

void MovableObject::OnCollisionEnter(std::shared_ptr<Collision> collider) {
	collisions.push_back(collider);
}

bool MovableObject::IsGrounded() {
	if (solidVertCollisionSides.y != 0) {
		return true;
	}
	return false;
}

void MovableObject::DoCollisionCheck() {
	solidVertCollisionSides.x = 0;
	solidVertCollisionSides.y = 0;
	solidHorCollisionSides.x = 0;
	solidHorCollisionSides.y = 0;

	if (collisions.size() > 0) {
		for (std::shared_ptr<Collision> col : collisions) {
			if(typeid(this) == typeid(PlayerObject))
				std::cout << "Col" << std::endl;
			sf::Vector2f collisionOverlaps = Level::GetObjectDistanceWithinAreaVector(collider.getGlobalBounds(), col->colliderObject->collider.getGlobalBounds());
			
			if (col->colliderObject->isSolid && col->collisionSides.y == 1 && collisionOverlaps.y > solidVertCollisionSides.y && collisionOverlaps.y <= col->colliderObject->collider.getGlobalBounds().height) { // BOTTOM collision
				solidVertCollisionSides.y = fmod(collisionOverlaps.y, collider.getGlobalBounds().height);
			}else if (col->colliderObject->isSolid && col->collisionSides.y == -1 && collisionOverlaps.y > solidVertCollisionSides.x && collisionOverlaps.y <= col->colliderObject->collider.getGlobalBounds().height) { // TOP collision
				solidVertCollisionSides.x = fmod(collisionOverlaps.y, collider.getGlobalBounds().height);
			}

			if (col->colliderObject->isSolid && col->collisionSides.x == -1 && collisionOverlaps.x > solidHorCollisionSides.y && collisionOverlaps.x <= col->colliderObject->collider.getGlobalBounds().width) { // RIGHT collision
				solidHorCollisionSides.y = collisionOverlaps.x;
			}
			else if (col->colliderObject->isSolid && col->collisionSides.x == 1 && collisionOverlaps.x > solidHorCollisionSides.x && collisionOverlaps.x <= col->colliderObject->collider.getGlobalBounds().width) { // LEFT collision
				solidHorCollisionSides.x = collisionOverlaps.x;
			}
		}
	}
}

void MovableObject::Move(sf::Vector2f movementVector) {
	movementVector = CalculateFinalMovementVector(movementVector);

	collider.move(movementVector);
	sprite->move(movementVector);

	pos += movementVector;
}

void MovableObject::CalculateMovement() {
	// Friction
	float friction = grounded ? groundFriction : airFriction;
	if (objectMovement.x > 0 && !inputRight) {
		if (objectMovement.x - groundSpeed * friction >= 0) {
			objectMovement.x -= groundSpeed * friction;
		}
		else {
			objectMovement.x = 0;
		}

	}
	else if (objectMovement.x < 0 && !inputLeft) {
		if (objectMovement.x + groundSpeed * friction <= 0) {
			objectMovement.x += groundSpeed * friction;
		}
		else {
			objectMovement.x = 0;
		}
	}

	// Garvity
	if (!grounded && objectMovement.y < gravity)
		objectMovement.y += gravity * Level::deltaTime;
	else if(grounded)
		objectMovement.y = 0;

	// Input Movement
	float speed = grounded ? groundSpeed : airSpeed;
	if (!blockMovement && inputLeft && objectMovement.x >= -speed) {
		objectMovement.x -= speed;
	}
	else if (!blockMovement && inputRight && objectMovement.x <= speed) {
		objectMovement.x += speed;
	}

	// Jump
	if (!blockMovement && inputJump && /*playerGrounded &&*/ jumpCooldown <= 0 && accumulatedJumpForce >= maxJumpForce) {
		objectMovement.y += jumpForce * jumpForceMultiplier;
		accumulatedJumpForce += jumpForce * jumpForceMultiplier;
		jumpForceMultiplier += 0.5;
		jumpCooldown = 0.01;
	}

	// Reset
	if ((!inputJump && accumulatedJumpForce != 0 && grounded)/* || (inputJump && accumulatedJumpForce <= maxJumpForce)*/) {
		//playerMovement.y += accumulatedJumpForce;
		accumulatedJumpForce = 0;
		jumpForceMultiplier = 1;
	}

	// Force Addition
	objectMovement.x += forceAddition.x;
	objectMovement.y += forceAddition.y;

	forceAddition.x = 0;
	forceAddition.y = 0;

	// Check if the object is "stuck" within another object & push it out
	if (solidHorCollisionSides.y > 1.1 && isSolid && objectMovement.x > 0) { // RIGHT
		objectMovement.x = 0;
		Move(sf::Vector2f(-solidHorCollisionSides.y-1, 0));
	}
	if (solidHorCollisionSides.x > 1.1 && isSolid && objectMovement.x < 0) { // LEFT
		objectMovement.x = 0;
		Move(sf::Vector2f(solidHorCollisionSides.x+1, 0));
	}

	if (solidVertCollisionSides.x > 1.1 && isSolid/* && objectMovement.y < 0*/) {//Above
		objectMovement.y = 0;
		Move(sf::Vector2f(0, solidVertCollisionSides.x+1));
	}
	else if (solidVertCollisionSides.y > 1.1 && isSolid/* && objectMovement.y > 0 */ ) {//Bottom
		objectMovement.y = 0;
		Move(sf::Vector2f(0, -solidVertCollisionSides.y-1));

	}
	//std::cout << std::to_string(solidVertCollisionSides.x) + " " + std::to_string(solidVertCollisionSides.y) << std::endl;

	// TODO: CHeck if its within the collider... move it out
}

void MovableObject::AddForce(sf::Vector2f force) {
	forceAddition += force;
}

void MovableObject::Destroy() {
	prefab = nullptr;
	if (collisions.size() > 0) {
		for (std::shared_ptr<Collision> col : collisions) {
			col.reset();
		}
	}
	collisions.clear();

	for (std::function<void()> func : OnRequestDestroy) {
		func();
	}
}

// Check for collisions around object and manipulate movementVector accordingly
sf::Vector2f MovableObject::CalculateFinalMovementVector(sf::Vector2f movementVector) {
	if (Level::instance == nullptr)
		return movementVector;

	int gridPosX = (int)((pos.x + (collider.getGlobalBounds().width / 2)) / Level::instance->tileSize); // Calculate Grid-Array position X of obj
	int gridPosY = (int)((pos.y + (collider.getGlobalBounds().height / 2)) / Level::instance->tileSize); // Calculate Grid-Array position Y of obj

	float objPosBottom = collider.getGlobalBounds().top + collider.getGlobalBounds().height;
	float objPosRight = collider.getGlobalBounds().left + collider.getGlobalBounds().width;

	std::vector<std::shared_ptr<GameObject>> goBelow = Level::instance->GetGameObjectsAtCoord(gridPosX, gridPosY + 1); // Gameobjects below
	std::vector<std::shared_ptr<GameObject>> goAbove = Level::instance->GetGameObjectsAtCoord(gridPosX, gridPosY - 1); // Gameobjects above
	std::vector<std::shared_ptr<GameObject>> goLeft = Level::instance->GetGameObjectsAtCoord(gridPosX-1, gridPosY); // Gameobjects left
	std::vector<std::shared_ptr<GameObject>> goRight = Level::instance->GetGameObjectsAtCoord(gridPosX+1, gridPosY); // Gameobjects Right

	// Check distance to objects below
	if (movementVector.y > 0 && goBelow.size() > 0) {
		for (std::shared_ptr<GameObject> go : goBelow) {
			if (go->isSolid) {
				float distanceToCollider = (go->collider.getGlobalBounds().top - objPosBottom);
				movementVector.y = (distanceToCollider < std::abs(movementVector.y)) ? distanceToCollider+.5 : movementVector.y;
			}
		}
	}

	// Check distance to objects above
	if (movementVector.y < 0 && goAbove.size() > 0) {
		for (std::shared_ptr<GameObject> go : goAbove) {
			if (go->isSolid) {
				float distanceToCollider = (collider.getGlobalBounds().top - (go->collider.getGlobalBounds().top+ go->collider.getGlobalBounds().height));
				movementVector.y = (distanceToCollider < std::abs(movementVector.y)) ? -distanceToCollider - .5 : movementVector.y;
			}
		}
	}

	// Check distance to objects on the left
	if (movementVector.x < 0 && goLeft.size() > 0) {
		for (std::shared_ptr<GameObject> go : goLeft) {
			if (go->isSolid) {
				float distanceToCollider = (collider.getGlobalBounds().left-(go->collider.getGlobalBounds().left + go->collider.getGlobalBounds().width));
				movementVector.x = (distanceToCollider < std::abs(movementVector.x)) ? -distanceToCollider - .5 : movementVector.x;
			}
		}
	}

	// Check distance to objects on the right
	if (movementVector.x > 0 && goRight.size() > 0) {
		for (std::shared_ptr<GameObject> go : goRight) {
			if (go->isSolid) {
				float distanceToCollider = ((go->collider.getGlobalBounds().left)-objPosRight);
				movementVector.x = (distanceToCollider < std::abs(movementVector.x)) ? distanceToCollider + .5 : movementVector.x;
			}
		}
	}

	return movementVector;
}