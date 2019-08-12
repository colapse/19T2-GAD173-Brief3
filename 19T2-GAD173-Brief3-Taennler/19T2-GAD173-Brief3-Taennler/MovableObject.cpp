#include "pch.h"
#include "MovableObject.h"

#ifndef Level
#include "Level.h"
#endif // !Level

#ifndef _IOSTREAM_
#include <iostream>
#endif // !_IOSTREAM_

MovableObject::MovableObject(float width, float height)
{
	collider = sf::RectangleShape(sf::Vector2f(width*0.97, height*0.97)); // *0.97 make collider bit smaller than the tile
}

MovableObject::MovableObject()
{
	objectMovement = sf::Vector2f(0, 0);
}


MovableObject::~MovableObject()
{
}

void MovableObject::Update() {
	DoCollisionCheck();
	grounded = IsGrounded();

	CalculateMovement();

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
			sf::Vector2f collisionOverlaps = Level::GetObjectDistanceWithinAreaVector(collider.getGlobalBounds(), col->colliderObject->collider.getGlobalBounds());
			
			if (col->colliderObject->isSolid && col->collisionSides.y == 1) { // BOTTOM collision
				solidVertCollisionSides.y = collisionOverlaps.y;// 1;
			}else if (col->colliderObject->isSolid && col->collisionSides.y == -1) { // TOP collision
				solidVertCollisionSides.x = collisionOverlaps.y;//1;
			}

			if (col->colliderObject->isSolid && col->collisionSides.x == 1) { // RIGHT collision
				solidHorCollisionSides.y = collisionOverlaps.x;// 1;
			}
			else if (col->colliderObject->isSolid && col->collisionSides.x == -1) { // LEFT collision
				solidHorCollisionSides.x = collisionOverlaps.x;// 1;
			}
		}
	}
}

void MovableObject::Move(sf::Vector2f movementVector) {
	collider.move(movementVector);
	sprite.move(movementVector);
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
	if (objectMovement.y < gravity)
		objectMovement.y += gravity * Level::deltaTime;

	// Input Movement
	float speed = grounded ? groundSpeed : airSpeed;
	if (inputLeft && objectMovement.x >= -speed) {
		objectMovement.x -= speed;
	}
	else if (inputRight && objectMovement.x <= speed) {
		objectMovement.x += speed;
	}

	// Jump
	if (inputJump && /*playerGrounded &&*/ jumpCooldown <= 0 && accumulatedJumpForce >= maxJumpForce) {

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

	/*
	// Border "Collisioncheck"
	sf::Vector2f WithinBounds = Level::IsObjectWithinAreaVector(collider.getGlobalBounds(), gameArea.getGlobalBounds());
	if (WithinBounds.x == -1 && playerMovement.x < 0) {
		playerMovement.x = 0;
	}
	else if (WithinBounds.x == 1 && playerMovement.x > 0) {
		playerMovement.x = 0;
	}
	if (WithinBounds.y == -1 && playerMovement.y < 0) {
		playerMovement.y = 0;
	}
	else if (WithinBounds.y == 1 && playerMovement.y > 0) {
		playerMovement.y = 0;
	}*/

	/*
	if (solidHorCollisionSides.x != 0)
		playerMovement.x = 0;
	if (solidVertCollisionSides.x != 0)
		playerMovement.y = 0;*/

	if (solidHorCollisionSides.x != 0 && objectMovement.x > 0) {
		objectMovement.x = 0;
		Move(sf::Vector2f(-solidHorCollisionSides.x, 0));
	}
	else if (solidHorCollisionSides.y != 0 && objectMovement.x < 0) {
		objectMovement.x = 0;
		Move(sf::Vector2f(solidHorCollisionSides.x, 0));
	}

	if (solidVertCollisionSides.x != 0 && objectMovement.y < 0) {
		objectMovement.y = 0;
		Move(sf::Vector2f(0, solidVertCollisionSides.y));
	}
	else if (solidVertCollisionSides.y != 0 && objectMovement.y > 0) {
		objectMovement.y = 0;
		Move(sf::Vector2f(0, -solidVertCollisionSides.y));

	}
	//std::cout << std::to_string(solidVertCollisionSides.x) + " " + std::to_string(solidVertCollisionSides.y) << std::endl;

	// TODO: CHeck if its within the collider... move it out
}