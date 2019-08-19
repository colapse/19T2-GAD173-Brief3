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
	collider = sf::RectangleShape(sf::Vector2f(width, height)); // Initiate collider of object
	if(isSolid)
		collider.setFillColor(sf::Color::Yellow); // For Debug
}

MovableObject::MovableObject()
{
	sprite = new sf::Sprite;
	collider = sf::RectangleShape(sf::Vector2f(32, 32)); // Initiate collider of object
	collider.setFillColor(sf::Color::Yellow); // For Debug
	objectMovement = sf::Vector2f(0, 0);
}


MovableObject::~MovableObject()
{
}

/** Method Update: This function is called every frame.*/
void MovableObject::Update() {
	DoCollisionCheck(); // Checks the collisions in this frame and calculates it's distance how far within the collider object he is
	grounded = IsGrounded(); // Checks if the object is grounded

	CalculateMovement(); // Calculates the movement distances in each direction

	Move(objectMovement * Level::deltaTime); // Moves object(Collider, sprite, ...) by the given movement vector

	// Counts down the jump cooldown by delta time
	if (jumpCooldown > 0) {
		jumpCooldown -= Level::deltaTime;
	}
}

void MovableObject::OnKeyUp(sf::Keyboard::Key key) {}

void MovableObject::OnKeyDown(sf::Keyboard::Key key) {}

/** Method OnCollisionEnter: This function is called if the object collides with a solid object. */
void MovableObject::OnCollisionEnter(std::shared_ptr<Collision> collider) {
	collisions.push_back(collider); // Store collision
}

/** Method IsGrounded: Checks if the object collides with a solid object on the ground. */
bool MovableObject::IsGrounded() {
	// Check if the object is within another solid object
	if (solidVertCollisionSides.y != 0) {
		return true;
	}
	return false;
}

/** Method DoCollisionCheck: Checks all collisions with this object within the current frame and calculate how far in the object is within the collider objects. */
void MovableObject::DoCollisionCheck() {
	// Reset collision overlap distances
	solidVertCollisionSides.x = 0; // Left Side Collision
	solidVertCollisionSides.y = 0; // Right Side Collision
	solidHorCollisionSides.x = 0; // Top Side Collision
	solidHorCollisionSides.y = 0; // Bottom Side Collision

	if (collisions.size() > 0) {
		// Iterate through the current collisions of this object
		for (std::shared_ptr<Collision> col : collisions) {
			// Calculate the how far in this object is within the collider object
			sf::Vector2f collisionOverlaps = Level::GetObjectDistanceWithinAreaVector(collider.getGlobalBounds(), col->colliderObject->collider.getGlobalBounds());
			
			// Calculate distances within collider objects (Depending on their sides)
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

/** Method Move: Performs a final validity check on the given movement vector and moves this objects (Better said it's components like collider, sprite, ...) by the amounts in the given movement vector. */
void MovableObject::Move(sf::Vector2f movementVector) {
	// Check the given movement vector if it's valid and correct the values if necessary. (This function makes a "Future" check to see if the object would be stuck within an object)
	movementVector = CalculateFinalMovementVector(movementVector);

	// Apply movement to collider and sprite component
	collider.move(movementVector);
	sprite->move(movementVector);

	// Update Position Vector
	pos += movementVector;
}

/** Method CalculateMovement: Calculates the movement vector from the given movement input signalsl, gravity, friction etc. */
void MovableObject::CalculateMovement() {
	// === Apply Friction
	// Define friction value depending on if the object is grounded or in air
	float friction = grounded ? groundFriction : airFriction;
	// Lower movement depending on the current movement direction
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

	// === Apply Gravity
	if (!grounded && objectMovement.y < gravity) // Increase gravity pull if the object isn't grounded an the maximum gravity isn't reached
		objectMovement.y += gravity * Level::deltaTime;
	else if(grounded)
		objectMovement.y = 0;

	// === Apply Input Movement
	// Calculate which speed needs to be applied depending on if the object is grounded or not
	float speed = grounded ? groundSpeed : airSpeed;
	if (!blockMovement && inputLeft && objectMovement.x >= -speed) { // Movement left
		objectMovement.x -= speed;
	}
	else if (!blockMovement && inputRight && objectMovement.x <= speed) { // Movement right
		objectMovement.x += speed;
	}

	// == Apply Input Jump
	if (!blockMovement && inputJump && jumpCooldown <= 0 && accumulatedJumpForce >= maxJumpForce) {


		objectMovement.y += jumpForce * jumpForceMultiplier;
		accumulatedJumpForce += jumpForce * jumpForceMultiplier;
		jumpForceMultiplier += 0.5;
		jumpCooldown = 0.001;
	}

	// Reset Jump
	if ((!inputJump && accumulatedJumpForce != 0 && grounded)) {
		accumulatedJumpForce = 0;
		jumpForceMultiplier = 1;
	}

	// === Apply Force Addition
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

	if (solidVertCollisionSides.x > 1.1 && isSolid) {//Above
		objectMovement.y = 0;
		Move(sf::Vector2f(0, solidVertCollisionSides.x+1));
	}
	else if (solidVertCollisionSides.y > 1.1 && isSolid) {//Bottom
		objectMovement.y = 0;
		Move(sf::Vector2f(0, -solidVertCollisionSides.y-1));

	}
}

/** Method AddForce: Adds a force to the objects movement. */
void MovableObject::AddForce(sf::Vector2f force) {
	forceAddition += force;
}

/** Method Destroy: Unloads some stuff from the object and informs listeners that it wants to be destroyed. */
void MovableObject::Destroy() {
	prefab = nullptr;
	sprite = nullptr;
	// Delete all collisions
	if (collisions.size() > 0) {
		for (std::shared_ptr<Collision> col : collisions) {
			col.reset();
		}
	}
	collisions.clear();

	// Call listener functions
	for (std::function<void()> func : OnRequestDestroy) {
		func();
	}
}

/** Method CalculateFinalMovementVector: Check for "future" collisions around object and manipulate movementVector accordingly */
sf::Vector2f MovableObject::CalculateFinalMovementVector(sf::Vector2f movementVector) {
	if (Level::instance == nullptr)
		return movementVector;

	int gridPosX = (int)((pos.x + (collider.getGlobalBounds().width / 2)) / Level::instance->tileSize); // Calculate Grid-Array position X of obj
	int gridPosY = (int)((pos.y + (collider.getGlobalBounds().height / 2)) / Level::instance->tileSize); // Calculate Grid-Array position Y of obj

	float objPosBottom = collider.getGlobalBounds().top + collider.getGlobalBounds().height;
	float objPosRight = collider.getGlobalBounds().left + collider.getGlobalBounds().width;

	// Getr lists of gameobjects surrounding the object
	std::vector<std::shared_ptr<GameObject>> goBelow = Level::instance->GetGameObjectsAtCoord(gridPosX, gridPosY + 1); // Gameobjects below
	std::vector<std::shared_ptr<GameObject>> goAbove = Level::instance->GetGameObjectsAtCoord(gridPosX, gridPosY - 1); // Gameobjects above
	std::vector<std::shared_ptr<GameObject>> goLeft = Level::instance->GetGameObjectsAtCoord(gridPosX-1, gridPosY); // Gameobjects left
	std::vector<std::shared_ptr<GameObject>> goRight = Level::instance->GetGameObjectsAtCoord(gridPosX+1, gridPosY); // Gameobjects Right

	// Check distance to objects below
	if (movementVector.y > 0 && goBelow.size() > 0) {
		for (std::shared_ptr<GameObject> go : goBelow) {
			if (go->isSolid) {
				// Calculate the distance between the gameobject and the buttom object
				float distanceToCollider = (go->collider.getGlobalBounds().top - objPosBottom);
				// Manipulates movement if the given vertical movement amount would get the object within the collider below
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