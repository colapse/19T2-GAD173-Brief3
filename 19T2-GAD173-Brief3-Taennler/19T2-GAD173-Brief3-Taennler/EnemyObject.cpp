#include "pch.h"
#include "EnemyObject.h"

#ifndef _IOSTREAM_
#include <iostream>
#endif // !_IOSTREAM_

#ifndef Level
#include "Level.h"
#endif // !Level

/** EnemyObject Constructor
 * @param width The width of the gameobject
 * @param height The height of the gameobject */
EnemyObject::EnemyObject(float width, float height)
{
	sprite = new sf::Sprite;
	collider = sf::RectangleShape(sf::Vector2f(width, height));
}
EnemyObject::EnemyObject()
{
	sprite = new sf::Sprite;
}


EnemyObject::~EnemyObject()
{
}

/** Method Start: This function is called right after the gameobject is created. */
void EnemyObject::Start() {
	// Set enemy specific stats
	jumpForce = -25;
	maxJumpForce = -300;
	groundSpeed = 30;
	airSpeed = 50;
	collider.setFillColor(sf::Color::Green); // For Debug
	inputLeft = true; // On spawn, the first movement direction of the enemy is left.
}

/** Method Update: This function is called every frame. */
void EnemyObject::Update() {
	DoCollisionCheck(); // Checks the collisions in this frame and calculates it's distance how far within the collider object he is
	grounded = IsGrounded(); // Checks if the object is grounded

	if(isAlive)
		AIInput(); // Apply AI movement to the enemy (If he's still alive) / (de-)Activate input

	CalculateMovement(); // Calculates the movement distances in each direction
	
	Move(objectMovement * Level::deltaTime); // Moves object(Collider, sprite, ...) by the given movement vector

	// Counts down the jump cooldown by delta time
	if (jumpCooldown > 0) {
		jumpCooldown -= Level::deltaTime;
	}
}

/** Method OnTriggerEnter: This function is called if the enemy collides with a NON-solid object. */
void EnemyObject::OnTriggerEnter(std::shared_ptr<Collision> collider) {
	if (isAlive && collider->colliderObject->tag == "Trap") { // Handle collisions with traps
		Die(); // Sets enemy as dead, blocks movement & changes enemy sprite

		// Add points to the player
		Level::instance->playerObject->AddPoints(5);

		// Destroy the trap
		collider->colliderObject->Destroy();

		return;
	}
	else if (isAlive && collider->colliderObject->tag == "2") { // Handle collision with laval
		// Calculate how far within the lava the enemy is
		sf::Vector2f dis = Level::GetObjectDistanceWithinAreaVector(this->collider.getGlobalBounds(), collider->colliderObject->collider.getGlobalBounds());

		// Check if the enemy is further than 5 pixels within the lava
		if (dis.x > 5 || dis.x < -5 || dis.y > 5 || dis.y < -5) {
			Die(); // Sets enemy as dead, blocks movement & changes enemy sprite
		}
		
	}
}

/** Method AIInput: This function is the "AI" of the enemy. It decides in which direction the enemy moves next and if he needs to jump. */
void EnemyObject::AIInput() {
	inputJump = false; // Reset Jump

	int gridPosX = (int)((pos.x+(collider.getGlobalBounds().width/2)) / Level::instance->tileSize); // Calculate Grid-Array position X of Enemy (Calculated from center point of enemy)
	int gridPosY = (int)((pos.y+(collider.getGlobalBounds().height / 2)) / Level::instance->tileSize); // Calculate Grid-Array position Y of Enemy (Calculated from center point of enemy)

	// Check the blocks surrounding the enemy
	std::vector<std::shared_ptr<GameObject>> goLeft = Level::instance->GetGameObjectsAtCoord(gridPosX-1, gridPosY); // Gameobjects on the tile left of the enemy
	std::vector<std::shared_ptr<GameObject>> goLeftTop = Level::instance->GetGameObjectsAtCoord(gridPosX-1, gridPosY-1); // Gameobjects on the tile left,top of the enemy
	std::vector<std::shared_ptr<GameObject>> goLeftBottom = Level::instance->GetGameObjectsAtCoord(gridPosX - 1, gridPosY + 1); // Gameobjects on the tile left,bottom of the enemy
	std::vector<std::shared_ptr<GameObject>> goLeftBottomX2Y2 = Level::instance->GetGameObjectsAtCoord(gridPosX - 2, gridPosY + 2); // Gameobjects on the tile leftx2,bottomx2 of the enemy
	std::vector<std::shared_ptr<GameObject>> goLeftBottomX1Y2 = Level::instance->GetGameObjectsAtCoord(gridPosX - 1, gridPosY + 2); // Gameobjects on the tile left,bottomx2 of the enemy
	std::vector<std::shared_ptr<GameObject>> goLeftBottomX2 = Level::instance->GetGameObjectsAtCoord(gridPosX - 2, gridPosY + 1); // Gameobjects on the tile leftx2,bottom of the enemy
	std::vector<std::shared_ptr<GameObject>> goLeftBottomX3 = Level::instance->GetGameObjectsAtCoord(gridPosX - 3, gridPosY + 1); // Gameobjects on the tile leftx3,bottom of the enemy

	std::vector<std::shared_ptr<GameObject>> goRight = Level::instance->GetGameObjectsAtCoord(gridPosX+1, gridPosY); // Gameobjects on the tile right of the enemy
	std::vector<std::shared_ptr<GameObject>> goRightTop = Level::instance->GetGameObjectsAtCoord(gridPosX+1, gridPosY-1); // Gameobjects on the tile right,top of the enemy
	std::vector<std::shared_ptr<GameObject>> goRightBottom = Level::instance->GetGameObjectsAtCoord(gridPosX+1, gridPosY+1); // Gameobjects on the tile right,bottom of the enemy
	std::vector<std::shared_ptr<GameObject>> goRightBottomX2Y2 = Level::instance->GetGameObjectsAtCoord(gridPosX+2, gridPosY+2); // Gameobjects on the tile rightx2,bottomx2 of the enemy
	std::vector<std::shared_ptr<GameObject>> goRightBottomX1Y2 = Level::instance->GetGameObjectsAtCoord(gridPosX+1, gridPosY+2); // Gameobjects on the tile right,bottomx2 of the enemy
	std::vector<std::shared_ptr<GameObject>> goRightBottomX2 = Level::instance->GetGameObjectsAtCoord(gridPosX + 2, gridPosY+1); // Gameobjects on the tile rightx2,bottom of the enemy
	std::vector<std::shared_ptr<GameObject>> goRightBottomX3 = Level::instance->GetGameObjectsAtCoord(gridPosX + 3, gridPosY+1); // Gameobjects on the tile rightx3,bottom of the enemy

	std::vector<std::shared_ptr<GameObject>> goBottomY2 = Level::instance->GetGameObjectsAtCoord(gridPosX, gridPosY + 2); // Gameobjects below the enemy

	// Check if the enemy is still in the same grid location as in last frame
	if (gridPosX == lastGridPosX) {
		aiStuckCounter += Level::deltaTime; // Count up time for which he was in that grid position

		// If the enemy is in the same grid location for more than a second, jump
		if (aiStuckCounter > 1) {
			inputJump = true;
		}

		// If the enemy is in the same grid location for more than 2.5 seconds, inverse movement direction
		if (aiStuckCounter > 2.5) {
			inputLeft = !inputLeft;
			inputRight = !inputRight;
		}
	}
	else {
		aiStuckCounter = 0;
	}

	// Update last horizontal grid position
	lastGridPosX = gridPosX;

	// Temporary states which will say if it's safe to move to specific locations
	bool saveLeft = false;
	bool saveLeftBottomY2 = false;
	bool saveRight = false;
	bool saveRightBottomY2 = false;

	///////// Check left side of the environment when moving left
	// If moving left & left side is out of the levels bounds change direction
	if (objectMovement.x < 0 && gridPosX-1 < 0) {
		saveLeft = false; // State that it's not save to move left
		inputLeft = false;
		inputRight = true;
	}
	
	// If moving left & there is ground on the left jump; Return;
	if (objectMovement.x < 0 && goLeft.size() > 0) {
		// Iterate through all objects contained on the left of the enemy
		for (std::shared_ptr<GameObject> go : goLeft) {
			// Check if the gameobject is a ground tile
			if (go->tag == "1") {
				// Check if there's another ground tile on top of the one on the left (=> 2 blocks high, don't jump then)
				if (goLeftTop.size() > 0) {
					bool breakLoop = false;
					// Iterate through all objects contained on the left,top of the enemy
					for (std::shared_ptr<GameObject> goLT : goLeftTop) {
						// Check if the gameobject is a ground tile, break both loops (Don't jump)
						if (goLT->tag == "1") {
							breakLoop = true;
							break;
						}
					}
					if (breakLoop)
						break;
				}
				
				// Ground Block on the left is only one high, jump
				inputJump = true;
				// Exit function; We don't want any further input changes.
				return;
			}
		}
	}

	// If moving left & there is lava below
	if (objectMovement.x < 0 && goLeftBottom.size() > 0) {
		for (std::shared_ptr<GameObject> go : goLeftBottom) {
			// Check if gameobject left,below is lava
			if (go->tag == "2") {
				// Check if the tile left+2,below is ground then jump over the lava
				if (goLeftBottomX2.size() > 0) {
					bool breakLoop = false;
					for (std::shared_ptr<GameObject> goLTX2 : goLeftBottomX2) {
						if (goLTX2->tag == "1") {
							inputJump = true;
							breakLoop = true;
							break;
						}
					}
					if (breakLoop)
						break;
				}
				// Check if the tile left+3,below is ground then jump over the lava
				if (goLeftBottomX3.size() > 0) {
					bool breakLoop = false;
					for (std::shared_ptr<GameObject> goLTX2 : goLeftBottomX3) {
						if (goLTX2->tag == "1") {
							inputJump = true;
							breakLoop = true;
							break;
						}
					}
					if (breakLoop)
						break;
				}

				// Invert movement direction
				inputLeft = false;
				inputRight = true;

				// Exit function; We don't want any further input changes.
				return;
			}
			else if (go->tag == "1") { // Check if gameobject left,below is ground
				saveLeft = true; // State that it's save to move to the left
			}
		}
	}

	// Check if moves left & there is ground 2 below
	if (objectMovement.x < 0 && goBottomY2.size() > 0) {
		for (std::shared_ptr<GameObject> go : goBottomY2) {
			// Check if the gameobject is a ground tile
			if (go->tag == "1") {
				saveLeftBottomY2 = true; // State that it's save 2 blocks below the enemy
			}
		}
	}

	// If moving left & there is air 2 below change movement direction; Return;
	if (!saveLeft && !saveLeftBottomY2 && objectMovement.x < 0 && goLeftBottomX1Y2.size() > 0) {
		for (std::shared_ptr<GameObject> go : goLeftBottomX1Y2) {
			// Check if left,belowx2 is air, move right
			if (go->tag == "0") {
				inputLeft = false;
				inputRight = true;
				return;
			}
		}
	}

	// If moving left & there is lava on the left change movement direction; Return;
	if (objectMovement.x < 0 && goLeft.size() > 0) {
		for (std::shared_ptr<GameObject> go : goLeft) {
			// Check if block is lava on the left, move right
			if (go->tag == "2") {
				inputLeft = false;
				inputRight = true;
				return;
			}
		}
	}

	///////////// Right Side Environment Checks (Same as above checks but for the right side)
	// If right side of the player is out of level change dir
	if (objectMovement.x > 0 && gridPosX >= Level::instance->gridSize.x) {
		saveRight = false;
		inputLeft = true;
		inputRight = false;
	}

	// If moving right & there is ground on the right jump; Return;
	if (objectMovement.x > 0 && goRight.size() > 0) {
		for (std::shared_ptr<GameObject> go : goRight) {
			if (go->tag == "1") {
				// Check if it's too high to jump; if yes break.
				if (goRightTop.size() > 0) {
					bool breakLoop = false;
					for (std::shared_ptr<GameObject> goRT : goRightTop) {
						if (goRT->tag == "1") {
							breakLoop = true;
							break;
						}
					}
					if (breakLoop)
						break;
				}

				inputJump = true;
				return;
			}
		}
	}

	// If moving right & there is lava below change movement direction or jump; Return;
	if (objectMovement.x > 0 && goRightBottom.size() > 0) {
		for (std::shared_ptr<GameObject> go : goRightBottom) {
			if (go->tag == "2") {
				// Check if the tile+2 is ground then jump over the lava
				if (goRightBottomX2.size() > 0) {
					bool breakLoop = false;
					for (std::shared_ptr<GameObject> goRTX2 : goRightBottomX2) {
						if (goRTX2->tag == "1") {
							inputJump = true;
							breakLoop = true;
							break;
						}
					}
					if (breakLoop)
						break;
				}
				// Check if the tile+3 is ground then jump over the lava
				if (goRightBottomX3.size() > 0) {
					bool breakLoop = false;
					for (std::shared_ptr<GameObject> goRTX2 : goRightBottomX3) {
						if (goRTX2->tag == "1") {
							inputJump = true;
							breakLoop = true;
							break;
						}
					}
					if (breakLoop)
						break;
				}
				inputLeft = true;
				inputRight = false;
				return;
			}
			else if (go->tag == "1") {
				saveRight = true;
			}
		}
	}

	// CHeck if there is ground 2 below on the right
	if (objectMovement.x > 0 && goBottomY2.size() > 0) {
		for (std::shared_ptr<GameObject> go : goBottomY2) {
			if (go->tag == "1") {
				saveRightBottomY2 = true;
			}
		}
	}
	// If moving right & there is air 2 below change movement direction; Return;
	if (!saveRight && !saveRightBottomY2 && objectMovement.x > 0 && goRightBottomX1Y2.size() > 0) {
		for (std::shared_ptr<GameObject> go : goRightBottomX1Y2) {
			if (go->tag == "0") {
				inputLeft = true;
				inputRight = false;
				return;
			}
		}
	}

	// If moving right & there is lava on the right change movement direction; Return;
	if (objectMovement.x > 0 && goRight.size() > 0) {
		for (std::shared_ptr<GameObject> go : goRight) {
			if (go->tag == "2") {
				inputLeft = true;
				inputRight = false;
				return;
			}
		}
	}
	
	///////////// Default collision checks (This part of the code is only executed if none of the ones above manipulated the movement)
	// Check for collisions, if collision change movement direction
	if (collisions.size() > 0) {
		for (std::shared_ptr<Collision> collision : collisions) {
			if (collision->collisionSides.x > 0) {
				inputRight = true;
				inputLeft = false;
			}else if (collision->collisionSides.x < 0) {
				inputRight = false;
				inputLeft = true;
			}
		}
	}
}

/** Method Die: Changes the enemys' texture, set it to dead and resets&blocks all input movement. */
void EnemyObject::Die() {
	// Change enemy texture to "EnemyDead"
	sprite->setTexture(*GameObjectPrefab::gameObjectPrefabTextures["EnemyDead"]);
	// Set enemy as dead
	isAlive = false;
	// Reset input movement
	inputLeft = false;
	inputRight = false;
	inputJump = false;
	blockMovement = true;
}
