#include "pch.h"
#include "PlayerObject.h"

#ifndef Level
#include "Level.h"
#endif // !Level

#ifndef EnemyObject
#include "EnemyObject.h"
#endif // !EnemyObject

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
	collider = sf::RectangleShape(sf::Vector2f(32, 32));
}


PlayerObject::~PlayerObject()
{
	Destroy();
	prefab = nullptr;
	sprite = nullptr;
	// Delete all collisions
	if (collisions.size() > 0) {
		for (std::shared_ptr<Collision> col : collisions) {
			col.reset();
		}
	}
	collisions.clear();

	playerCamera.reset();
	playerCamera = nullptr;
}

/** Method SetPosition(): Sets the position of the players collider, sprite and camera. */
void PlayerObject::SetPosition(sf::Vector2f pos) {
	this->pos = pos;
	collider.setPosition(pos);
	if(sprite != nullptr)
		sprite->setPosition(pos);
	if(playerCamera != nullptr)
		playerCamera->setCenter(pos);
}

/** Method Start(): Gets called when the player is added to the world. */
void PlayerObject::Start() {
	collider.setFillColor(sf::Color::Green); // For Debug
}

/** Method Update(): Gets called every frame. Handles collisions, grounded checks and movement. */
void PlayerObject::Update() {
	DoCollisionCheck(); // Checks all occured collisions and calculates their overlaps
	grounded = IsGrounded(); // Checks wheither  the player is grounded or not
	
	CalculateMovement(); // Calculates the movement depending on different factors (Friction, input, gravity, ...)

	Move(objectMovement * Level::deltaTime); // Moves the player by a fraction of the calculated movement vector)

	// Count down Jump Cooldown
	if (jumpCooldown > 0) {
		jumpCooldown -= Level::deltaTime;
	}

	// Check for enemy collision
	if (collisions.size() > 0) {
		// Iterate through all occured collisions in this frame
		for(std::shared_ptr<Collision> collision : collisions) {
			// Check if the collider is an enemy
			if (isAlive && collision->colliderObject->tag == "Enemy" && typeid(*collision->colliderObject) == typeid(EnemyObject)) {
				std::shared_ptr<EnemyObject> enemyObj = std::dynamic_pointer_cast<EnemyObject>(collision->colliderObject);
				// Check if the enemy is still alive
				if (enemyObj->isAlive) { // Player dies
					isAlive = false;
					blockMovement = true;
					// TODO: Show game over screen
				}
			}
		}
	}
}

/** Method OnKeyUp(): Gets called when a Key is released. Depending on the key that is released, input will be captured. */
void PlayerObject::OnKeyUp(sf::Keyboard::Key key) {
	switch (key) {
	case sf::Keyboard::Key::Left:
	case sf::Keyboard::Key::A:
		inputLeft = false; // Input Move Left -> false
		break;
	case sf::Keyboard::Key::Right:
	case sf::Keyboard::Key::D:
		inputRight = false; // Input Move Right -> false
		break;
	case sf::Keyboard::Key::Space:
		inputJump = false; // Input Jump -> false
		break;
	case sf::Keyboard::Key::Enter:
		if(isAlive)
			ThrowTrap(); // Throw a trap
		break;
	}
}

/** Method OnKeyDown(): Gets called when a Key is pressed. Depending on the key that is pressed, input will be captured. */
void PlayerObject::OnKeyDown(sf::Keyboard::Key key) {
	switch (key) {
	case sf::Keyboard::Key::Left:
	case sf::Keyboard::Key::A:
		inputLeft = true; // Input indicating player should move left
		lookDirection.x = -1; // Set look direction
		break;
	case sf::Keyboard::Key::Right:
	case sf::Keyboard::Key::D:
		inputRight = true; // Input indicating player should move right
		lookDirection.x = 1; // Set look direction
		break;
	case sf::Keyboard::Key::Space:
		inputJump = true; // Input indicating player should jump
		break;
	}
}

/** Method Move(): Moves the player according to the given movement vector. */
void PlayerObject::Move(sf::Vector2f movementVector) {
	// Check the given movement vector if it's valid and correct the values if necessary. (This function makes a "Future" check to see if the object would be stuck within an object)
	movementVector = CalculateFinalMovementVector(movementVector);

	// Move components
	collider.move(movementVector);
	sprite->move(movementVector);
	playerCamera->move(movementVector);

	// Store new position of the player
	this->pos = collider.getPosition();
}

/** Method OnTriggerEnter(): Gets called when a non-solid object collides with the player. */
void PlayerObject::OnTriggerEnter(std::shared_ptr<Collision> collider) {
	if (isAlive && collider->colliderObject->tag == "Coin") { // Colided with a coin
		AddCoins(1); // Increases the coin count & notifies listeners
		collider->colliderObject->Destroy(); // Destroy the collider object (coin)
		return;
	}
	else if (isAlive && collider->colliderObject->tag == "2") { // Collided with lava
		// Calculate how far in the player is within the lava object
		sf::Vector2f dis = Level::GetObjectDistanceWithinAreaVector(this->collider.getGlobalBounds(), collider->colliderObject->collider.getGlobalBounds());

		// If the player is more than 3 pixels within the lava object, he dies
		if (dis.x > 3 || dis.x < -3 || dis.y > 3 || dis.y < -3) {
			isAlive = false;
			blockMovement = true;
			// TODO: Show gameover screen
		}
	}
	else if (isAlive && collider->colliderObject->tag == "Exit") { // Colided with exit
		blockMovement = true;
		Level::instance->playerReachedEnd = true; // States that the player reached the end
	}
}

/** Method AddCoins(): Adds the given amount to the coin count and notifies all listeners that the coin count changed. */
void PlayerObject::AddCoins(int amount) {
	coins += amount; // Increase coin count

	// Notify all listeners that the coin count changed
	if (OnCoinAmountChange.size() > 0) {
		for (std::function<void(int)> func : OnCoinAmountChange) {
			func(coins);
		}
	}
}

/** Method GetCoinCount(): Returns the coin count of the player. */
int PlayerObject::GetCoinCount() {
	return coins;
}

/** Method AddPoints(): Adds the given amount of points to the score count and notifies all listeners that the score changed. */
void PlayerObject::AddPoints(int amount) {
	score += amount; // Increase score by given amount

	// Notify listeners that score changed
	if (OnScoreChange.size() > 0) {
		for (std::function<void(int)> func : OnScoreChange) {
			func(score);
		}
	}
}

/** Method GetScore(): Returns the score of the player. */
int PlayerObject::GetScore() {
	return score;
}

/** Method ThrowTrap(): Instantiates a Trap object and "throws" it in the current look-direction of the player. */
void PlayerObject::ThrowTrap() {
	if (Level::instance == nullptr)
		return;

	// Create new MovableObject (=> The Trap)
	std::shared_ptr<MovableObject> trapObj = std::make_shared<MovableObject>();
	
	trapObj->prefab = GameObjectPrefab::gameObjectPrefabs["Trap"];
	trapObj->sprite->setTexture(*GameObjectPrefab::gameObjectPrefabTextures["Trap"]);

	sf::Vector2f spawnPos((pos.x + (int)ceil(lookDirection.x)*32), pos.y - 16); // Calculate Spawn Position
	trapObj->SetPosition(spawnPos);
	trapObj->isSolid = GameObjectPrefab::gameObjectPrefabs["Trap"]->isSolid;
	trapObj->tag = "Trap";
	trapObj->renderLayer = 3;

	// Add the Trap to the world
	Level::instance->AddGameObject(trapObj);
	
	// Add a "throw" force to the trap
	sf::Vector2f force = sf::Vector2f(80,-.5f*gravity);
	force.x *= lookDirection.x;
	force.y *= (lookDirection.y==0?1: lookDirection.y);

	trapObj->AddForce(force);
}

/** Method Destroy(): Notifies all listeners that the object needs to be destroyed. */
void PlayerObject::Destroy() {
	for (std::function<void()> func : OnRequestDestroy) {
		func();
	}
}