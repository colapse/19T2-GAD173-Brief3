#include "pch.h"
#include "EnemyObject.h"

#ifndef _IOSTREAM_
#include <iostream>
#endif // !_IOSTREAM_

#ifndef Level
#include "Level.h"
#endif // !Level

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

void EnemyObject::Start() {
	jumpForce = -25;
	maxJumpForce = -300;
	groundSpeed = 30;
	airSpeed = 50;
	collider.setFillColor(sf::Color::Green); // For Debug
	inputLeft = true;
}

void EnemyObject::Update() {
	DoCollisionCheck();
	grounded = IsGrounded();

	if(isAlive)
		AIInput();

	CalculateMovement();

	Move(objectMovement * Level::deltaTime);

	//Cooldown
	if (jumpCooldown > 0) {
		jumpCooldown -= Level::deltaTime;
	}
}

void EnemyObject::OnTriggerEnter(std::shared_ptr<Collision> collider) {
	if (isAlive && collider->colliderObject->tag == "Trap") {
		sprite->setTexture(*GameObjectPrefab::gameObjectPrefabTextures["EnemyDead"]);
		isAlive = false;
		inputLeft = false;
		inputRight = false;
		inputJump = false;
		Level::instance->playerObject->AddPoints(5);

		collider->colliderObject->Destroy();
		return;
	}
	else if (isAlive && collider->colliderObject->tag == "2") {
		sf::Vector2f dis = Level::GetObjectDistanceWithinAreaVector(this->collider.getGlobalBounds(), collider->colliderObject->collider.getGlobalBounds());

		if (dis.x > 5 || dis.x < -5 || dis.y > 5 || dis.y < -5) {
			sprite->setTexture(*GameObjectPrefab::gameObjectPrefabTextures["EnemyDead"]);
			isAlive = false;
			inputLeft = false;
			inputRight = false;
			inputJump = false;
			blockMovement = true;
		}
		
	}
}

void EnemyObject::AIInput() {
	inputJump = false; // Reset Jump

	int gridPosX = (int)((pos.x+(collider.getGlobalBounds().width/2)) / Level::instance->tileSize); // Calculate Grid-Array position X of Enemy (Calculated from center point of enemy)
	int gridPosY = (int)((pos.y+(collider.getGlobalBounds().height / 2)) / Level::instance->tileSize); // Calculate Grid-Array position Y of Enemy (Calculated from center point of enemy)
	std::vector<std::shared_ptr<GameObject>> goLeft = Level::instance->GetGameObjectsAtCoord(gridPosX-1, gridPosY);
	std::vector<std::shared_ptr<GameObject>> goLeftTop = Level::instance->GetGameObjectsAtCoord(gridPosX-1, gridPosY-1);
	std::vector<std::shared_ptr<GameObject>> goLeftBottom = Level::instance->GetGameObjectsAtCoord(gridPosX - 1, gridPosY + 1);
	std::vector<std::shared_ptr<GameObject>> goLeftBottomX2Y2 = Level::instance->GetGameObjectsAtCoord(gridPosX - 2, gridPosY + 2);
	std::vector<std::shared_ptr<GameObject>> goLeftBottomX1Y2 = Level::instance->GetGameObjectsAtCoord(gridPosX - 1, gridPosY + 2);
	std::vector<std::shared_ptr<GameObject>> goLeftBottomX2 = Level::instance->GetGameObjectsAtCoord(gridPosX - 2, gridPosY + 1);
	std::vector<std::shared_ptr<GameObject>> goLeftBottomX3 = Level::instance->GetGameObjectsAtCoord(gridPosX - 3, gridPosY + 1);
	std::vector<std::shared_ptr<GameObject>> goRight = Level::instance->GetGameObjectsAtCoord(gridPosX+1, gridPosY);
	std::vector<std::shared_ptr<GameObject>> goRightTop = Level::instance->GetGameObjectsAtCoord(gridPosX+1, gridPosY-1);
	std::vector<std::shared_ptr<GameObject>> goRightBottom = Level::instance->GetGameObjectsAtCoord(gridPosX+1, gridPosY+1);
	std::vector<std::shared_ptr<GameObject>> goRightBottomX2Y2 = Level::instance->GetGameObjectsAtCoord(gridPosX+2, gridPosY+2);
	std::vector<std::shared_ptr<GameObject>> goRightBottomX1Y2 = Level::instance->GetGameObjectsAtCoord(gridPosX+1, gridPosY+2);
	std::vector<std::shared_ptr<GameObject>> goRightBottomX2 = Level::instance->GetGameObjectsAtCoord(gridPosX + 2, gridPosY+1);
	std::vector<std::shared_ptr<GameObject>> goRightBottomX3 = Level::instance->GetGameObjectsAtCoord(gridPosX + 3, gridPosY+1);

	std::vector<std::shared_ptr<GameObject>> goBottomY2 = Level::instance->GetGameObjectsAtCoord(gridPosX, gridPosY + 2);

	if (gridPosX == lastGridPosX) {
		aiStuckCounter += Level::deltaTime;

		if (aiStuckCounter > 1) {
			inputJump = true;
		}
		if (aiStuckCounter > 2.5) {
			inputLeft = !inputLeft;
			inputRight = !inputRight;
		}
	}
	else {
		aiStuckCounter = 0;
	}
	lastGridPosX = gridPosX;

	bool saveLeft = false;
	//bool saveLeftBottom = true;
	bool saveLeftBottomY2 = false;
	//bool saveLeftTop = true;
	bool saveRight = false;
	//bool saveRightBottom = false;
	bool saveRightBottomY2 = false;
	//bool saveRightTop = true;

	//std::cout << "curX: "+std::to_string(gridPosX)+ " curY: " + std::to_string(gridPosY) + "GoLeft Size: " + std::to_string(goLeft.size()) << std::endl;
	///////////// Left Side Environment Checks

	// If left is out of level change dir
	if (objectMovement.x < 0 && gridPosX-1 < 0) {
		saveLeft = false;
		inputLeft = false;
		inputRight = true;
	}
	
	// If moving left & there is ground on the left jump; Return;
	if (objectMovement.x < 0 && goLeft.size() > 0) {
		for (std::shared_ptr<GameObject> go : goLeft) {
			//std::cout << "Tag: "+go->tag << std::endl;
			if (go->tag == "1") {
				// Check if it's too high to jump; if yes break.
				if (goLeftTop.size() > 0) {
					bool breakLoop = false;
					for (std::shared_ptr<GameObject> goLT : goLeftTop) {
						if (goLT->tag == "1") {
							breakLoop = true;
							break;
						}
					}
					if (breakLoop)
						break;
				}
				//std::cout << "GROUND!" << std::endl;
				inputJump = true;
				return;
			}
		}
	}

	// If moving left & there is lava below change movement direction; Return;
	if (objectMovement.x < 0 && goLeftBottom.size() > 0) {
		for (std::shared_ptr<GameObject> go : goLeftBottom) {
			if (go->tag == "2") {
				// Check if the tile+2 is ground then jump over the lava
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
				// Check if the tile+3 is ground then jump over the lava
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
				inputLeft = false;
				inputRight = true;
				return;
			}
			else if (go->tag == "1") {
				saveLeft = true;
			}
		}
	}

	// Check if there is ground on the left 2 below
	if (objectMovement.x < 0 && goBottomY2.size() > 0) {
		for (std::shared_ptr<GameObject> go : goBottomY2) {
			if (go->tag == "1") {
				saveLeftBottomY2 = true;
			}
		}
	}
	// If moving left & there is air 2 below change movement direction; Return;
	if (!saveLeft && !saveLeftBottomY2 && objectMovement.x < 0 && goLeftBottomX1Y2.size() > 0) {
		for (std::shared_ptr<GameObject> go : goLeftBottomX1Y2) {
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
			if (go->tag == "2") {
				inputLeft = false;
				inputRight = true;
				return;
			}
		}
	}

	///////////// Right Side Environment Checks
	// If right is out of level change dir
	if (objectMovement.x > 0 && gridPosX >= Level::instance->gridSize.x) {
		saveRight = false;
		inputLeft = true;
		inputRight = false;
	}
	// If moving right & there is ground on the right jump; Return;
	if (objectMovement.x > 0 && goRight.size() > 0) {
		for (std::shared_ptr<GameObject> go : goRight) {
			//std::cout << "Tag: "+go->tag << std::endl;
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
				//std::cout << "GROUND!" << std::endl;
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
	
	// COLLISION CHECKS
	// Check for collisions
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
