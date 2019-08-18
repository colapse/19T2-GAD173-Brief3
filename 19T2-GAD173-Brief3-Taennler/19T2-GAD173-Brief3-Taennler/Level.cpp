#include "pch.h"
#include "Level.h"
#ifndef fstream
#include <fstream>
#endif // !fstream
#ifndef regex
#include <regex>
#endif // !regex
#ifndef sstream
#include <sstream>
#endif // !sstream
#ifndef filesystem
#include <filesystem>
#endif // !filesystem
#ifndef GameObjectPrefab
#include "GameObjectPrefab.h"
#endif // !GameObjectType
#ifndef AnimatedSprite
#include "AnimatedSprite.h"
#endif // !AnimatedSprite



#ifndef _IOSTREAM_
#include <iostream>
#endif // !_IOSTREAM_

const float Level::defaultTileSize = 32;
float Level::deltaTime = 0.01;
std::shared_ptr<Level> Level::instance = nullptr;

std::vector<std::string> SplitStringByDeli(std::string stringToSplit, char delimeter);// TODO

Level::Level(std::vector<std::vector<std::vector<std::string>>> pRawGrid, float pTileSize, std::string pLevelName) : rawGrid(pRawGrid), tileSize(pTileSize), levelName(pLevelName) {
	deltaTime = 0.01;
	gridSize.y = pRawGrid.size();
	if (gridSize.y > 0)
		gridSize.x = pRawGrid[0].size();
	InitializeGameObjectGrid();
}

Level::Level()
{
	deltaTime = 0.01;
}

Level::~Level()
{
	deltaTime = 0.01;
}

void Level::RemoveGameObject(std::shared_ptr<GameObject> gameObject) {
	gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), gameObject), gameObjects.end());

	if (typeid(*gameObject) == typeid(MovableObject)) {
		movableObjects.erase(std::remove(movableObjects.begin(), movableObjects.end(), gameObject), movableObjects.end());
	}

	if(gameObjectsDrawLayer.find(gameObject->renderLayer) != gameObjectsDrawLayer.end())
		gameObjectsDrawLayer[gameObject->renderLayer].erase(std::remove(gameObjectsDrawLayer[gameObject->renderLayer].begin(), gameObjectsDrawLayer[gameObject->renderLayer].end(), gameObject), gameObjectsDrawLayer[gameObject->renderLayer].end());
}
/*
void Level::RemoveMovableObject(std::shared_ptr<MovableObject> gameObject) {
	gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), gameObject), gameObjects.end());
	movableObjects.erase(std::remove(movableObjects.begin(), movableObjects.end(), gameObject), movableObjects.end());
	if (gameObjectsDrawLayer.find(gameObject->renderLayer) != gameObjectsDrawLayer.end())
		gameObjectsDrawLayer[gameObject->renderLayer].erase(std::remove(gameObjectsDrawLayer[gameObject->renderLayer].begin(), gameObjectsDrawLayer[gameObject->renderLayer].end(), gameObject), gameObjectsDrawLayer[gameObject->renderLayer].end());
	
}*/

void Level::InitializeGameObjectGrid() {
	gameObjectGrid = std::vector<std::vector<std::vector<std::shared_ptr<GameObject>>>>(gridSize.y, std::vector<std::vector<std::shared_ptr<GameObject>>>(gridSize.x, std::vector<std::shared_ptr<GameObject>>(0,nullptr)));
	gameObjects = std::vector<std::shared_ptr<GameObject>>(0, nullptr);

	for (int y = 0; y < rawGrid.size(); y++) {
		for (int x = 0; x < rawGrid[y].size(); x++) {
			for (std::string s : rawGrid[y][x]) {
				if (GameObjectPrefab::IsValidGOPrefabChar(s)) {
					// TODO: Check if is GameObject or MovableObject!

					// Differentiate between Gameobject type
					if (GameObjectPrefab::gameObjectPrefabs[s]->isStatic) {// Simple GameObject
						std::shared_ptr<GameObject> gameobj_ptr = std::make_shared<GameObject>(tileSize, tileSize);
						gameobj_ptr->prefab = GameObjectPrefab::gameObjectPrefabs[s];
						
						if (GameObjectPrefab::gameObjectPrefabs[s]->isAnimatedSprite) {
							AnimatedSprite * animSprite = new AnimatedSprite();
							if (s == "2") {
								animSprite->frameCountX = 5;
								animSprite->frameCountY = 1;
								animSprite->tileSize = tileSize;
							}
							else if (s == "Coin") {
								animSprite->frameCountX = 5;
								animSprite->frameCountY = 4;
								animSprite->tileSize = tileSize;
							}
							if (gameobj_ptr->sprite != nullptr)
								delete gameobj_ptr->sprite;

							gameobj_ptr->sprite = animSprite;
						}

						gameobj_ptr->sprite->setTexture(*GameObjectPrefab::gameObjectPrefabTextures[s]);
						gameobj_ptr->SetPosition(sf::Vector2f(x*tileSize, y*tileSize));
						gameobj_ptr->isSolid = GameObjectPrefab::gameObjectPrefabs[s]->isSolid;
						gameobj_ptr->renderLayer = 0;

						// Set Tag
						gameobj_ptr->tag = s;

						gameObjectGrid[y][x].push_back(gameobj_ptr);

						AddGameObject(gameobj_ptr);
					}
					else {
						if (s == "Player") { // Player (Don't spawn him yet
							playerSpawnLocation = sf::Vector2f(x*tileSize, y*tileSize);
						}
						else if (s == "Enemy") {
							std::shared_ptr<EnemyObject> gameobj_ptr = std::make_shared<EnemyObject>(tileSize, tileSize);
							gameobj_ptr->prefab = GameObjectPrefab::gameObjectPrefabs[s];
							gameobj_ptr->sprite->setTexture(*GameObjectPrefab::gameObjectPrefabTextures[s]);
							gameobj_ptr->SetPosition(sf::Vector2f(x*tileSize, y*tileSize));
							gameobj_ptr->isSolid = GameObjectPrefab::gameObjectPrefabs[s]->isSolid;
							gameobj_ptr->renderLayer = 2;

							// Set Tag
							gameobj_ptr->tag = s;
							AddGameObject(gameobj_ptr);
						}
						else {// MovableObject
							std::shared_ptr<MovableObject> gameobj_ptr = std::make_shared<MovableObject>(tileSize, tileSize);
							//gameobj_ptr->sprite = sf::Sprite(*GameObjectPrefab::gameObjectPrefabTextures[s]);
							gameobj_ptr->prefab = GameObjectPrefab::gameObjectPrefabs[s];

							if (GameObjectPrefab::gameObjectPrefabs[s]->isAnimatedSprite) {
								AnimatedSprite * animSprite = new AnimatedSprite();
								if (s == "2") {
									animSprite->frameCountX = 5;
									animSprite->frameCountY = 1;
									animSprite->tileSize = tileSize;
								}
								else if (s == "Coin") {
									animSprite->frameCountX = 5;
									animSprite->frameCountY = 4;
									animSprite->tileSize = tileSize;
								}
								if (gameobj_ptr->sprite != nullptr)
									delete gameobj_ptr->sprite;
								gameobj_ptr->sprite = animSprite;
							}

							gameobj_ptr->sprite->setTexture(*GameObjectPrefab::gameObjectPrefabTextures[s]);
							gameobj_ptr->SetPosition(sf::Vector2f(x*tileSize, y*tileSize));
							gameobj_ptr->isSolid = GameObjectPrefab::gameObjectPrefabs[s]->isSolid;
							gameobj_ptr->renderLayer = 1;

							// Set Tag
							gameobj_ptr->tag = s;
							AddGameObject(gameobj_ptr);
						}
					}
				}
			}
		}
	}
}

std::vector<std::shared_ptr<GameObject>> Level::GetGameObjectsAtCoord(unsigned int x, unsigned int y) {
	if (gameObjectGrid.size() > y && gameObjectGrid[y].size() > x)
		return gameObjectGrid[y][x];
}

void Level::AddGameObjectAtCoord(unsigned int x, unsigned int y, GameObjectPrefab prefab) {

}

void Level::DoCollisionChecks() {
	if (movableObjects.size() == 0)
		return;

	for (std::shared_ptr<MovableObject> movableObject : movableObjects) {
		if (movableObject == nullptr)
			continue;

		// Delete old collisions
		
		if (movableObject->collisions.size() > 0) {
			for (std::shared_ptr<Collision> col : movableObject->collisions) {
				col.reset();
			}
		}
		movableObject->collisions.clear();

		movableObject->solidHorCollisionSides = sf::Vector2f(0, 0);
		movableObject->solidVertCollisionSides = sf::Vector2f(0, 0);
		for (std::shared_ptr<GameObject> gameObject : gameObjects) {
			if (gameObject != nullptr && gameObject != movableObject) {
				sf::Vector2f collisions = GetCollisionSideVector(movableObject->collider.getGlobalBounds(), gameObject->collider.getGlobalBounds());

				if (collisions.x != 0 || collisions.y != 0) {
					// Notify Collision for movable object
					auto movCol = std::make_shared<Collision>();
					movCol->colliderObject = gameObject;
					movCol->collisionSides = collisions;

					if (gameObject->isSolid) {
						//std::cout << "Collision... Solid" << std::endl;
						movableObject->OnCollisionEnter(movCol);
					}
					else {
						//std::cout << "Collision... Trigger" << std::endl;
						movableObject->OnTriggerEnter(movCol);
					}

					// Invert collision side vector
					sf::Vector2f collisionsInverted = collisions;
					collisionsInverted.x *= -1;
					collisionsInverted.y *= -1;

					// Notify Collision for collider gameobject object
					auto goCol = std::make_shared<Collision>();
					goCol->colliderObject = movableObject;
					goCol->collisionSides = collisionsInverted;

					if (movableObject->isSolid)
						gameObject->OnCollisionEnter(goCol);
					else
						gameObject->OnTriggerEnter(goCol);
				}
			}
		}
	}
}

/** GetCollisionSideVector: Checks whether two objects are colliding and on which side the collision occured. The collision sides refer to the given objectOne.
 * @param objectOne The main objects bounds
 * @param collider The colliders bounds
 * @return x=-1: left, x=1: right, y=-1: top, y=1: bottom, x/y=0: no collision
*/
sf::Vector2f Level::GetCollisionSideVector(sf::FloatRect objectOne, sf::FloatRect collider) {

	sf::Vector2f collisionSides(0, 0); // Init collision sides vector with 0

	// Check if there is a collision at all with the collider
	if (objectOne.intersects(collider)) {

		// Calculate the center of objectOne
		sf::Vector2f o1Center(objectOne.left + objectOne.width / 2, objectOne.top + objectOne.height / 2);
		// Calculate the center of the collider
		sf::Vector2f colCenter(collider.left + collider.width / 2, collider.top + collider.height / 2);

		// Calculate PI
		const double PI = atan(1) * 4;
		// Calculate delta X/Y of objectOnes' & the colliders' centers
		double deltaX = colCenter.x - o1Center.x;
		double deltaY = colCenter.y - o1Center.y;
		// Calculate the angle between the two objects position. (0 Degree = X position of colliders center on the left; Clockwise; 0-360 Degree;
		double angle = (std::atan2(deltaY, deltaX) * (180 / PI));

		if (angle < 0) {
			angle += 360; // Make sure the angles value is from 0-360
		}

		// Calculate the angles between the colliders corners and its center point
		double cornerLeftTopAngle = std::atan2(colCenter.y - collider.top, colCenter.x - collider.left) * 180 / PI;
		double cornerRightTopAngle = (180 - cornerLeftTopAngle);
		double cornerLeftBottomAngle = 360 - cornerLeftTopAngle;
		double cornerRightBottomAngle = 180 + cornerLeftTopAngle;

		// Left collision
		if (angle < cornerLeftTopAngle || angle > cornerLeftBottomAngle) {
			collisionSides.x = -1;
		}
		// Bottom collision
		if (angle > cornerLeftTopAngle && angle < cornerRightTopAngle) {
			collisionSides.y = 1;
		}
		// Right collision
		if (angle > cornerRightTopAngle && angle < cornerRightBottomAngle) {
			collisionSides.x = 1;
		}
		// Top collision
		if (angle > cornerRightBottomAngle && angle < cornerLeftBottomAngle) {
			collisionSides.y = -1;
		}

		/* TODO: Corner Collisions
		if(angle == cornerLeftTopAngle)
			std::cout << "cornerLeftTopAngle" << std::endl;
		if (angle == cornerRightTopAngle)
			std::cout << "cornerRightTopAngle" << std::endl;
		if (angle == cornerLeftBottomAngle)
			std::cout << "cornerLeftBottomAngle" << std::endl;
		if (angle == cornerRightBottomAngle)
			std::cout << "cornerRightBottomAngle" << std::endl;
		*/

	}


	return collisionSides;
}

/** GetObjectDistanceWithinAreaVector: Checks wheither the object is within an area and the distance how far in it is.
 * @return sf::Vector2f x=negative: left, x=positive: right, y=negative: top, y=positive: bottom, x/y=0: not in bounds
*/
sf::Vector2f Level::GetObjectDistanceWithinAreaVector(sf::FloatRect objectBounds, sf::FloatRect areaBounds) {
	sf::Vector2f outOfBoundsVec(0, 0);
	//float skin = 10;
	// Check if object is within the area
	sf::Vector2f collisionSides = Level::GetCollisionSideVector(objectBounds, areaBounds);
	if (collisionSides.x == 0 && collisionSides.y == 0)
		return outOfBoundsVec;

	// Check Left Side of object
	if (collisionSides.x == 1 && areaBounds.left + areaBounds.width > objectBounds.left) {
		float distance = std::abs(areaBounds.left + areaBounds.width - objectBounds.left);
		outOfBoundsVec.x = distance;// std::abs(distance) < areaBounds.width ? distance : 0;
		//std::cout << std::to_string(areaBounds.left) + " + " + std::to_string(areaBounds.width) + " - " + std::to_string(objectBounds.left) << std::endl;
	}else if (collisionSides.x == -1 && objectBounds.left + objectBounds.width > areaBounds.left){// Check Right Side of object
		float distance = std::abs(objectBounds.left + objectBounds.width - areaBounds.left);
		outOfBoundsVec.x = distance;// std::abs(distance) < areaBounds.width ? distance : 0;
	}

	// Check Top Side of object
	if (collisionSides.y == -1 && objectBounds.top < areaBounds.top + areaBounds.height)
		outOfBoundsVec.y = std::abs(areaBounds.top + areaBounds.height)- std::abs(objectBounds.top);
	else if (collisionSides.y == 1 && objectBounds.top+objectBounds.height > areaBounds.top)// Check Bottom Side of object
		outOfBoundsVec.y = std::abs(objectBounds.top + objectBounds.height) - std::abs(areaBounds.top);
	return outOfBoundsVec;
}

/** IsObjectWithinAreaVector: Checks whether an object is within another one & returns a vector indicating on which side it is out of bounds
 * @return sf::Vector2f x=-1: left, x=1: right, y=-1: top, y=1: bottom, x/y=0: in bounds
*/
sf::Vector2f Level::IsObjectWithinAreaVector(sf::FloatRect objectBounds, sf::FloatRect areaBounds) {
	sf::Vector2f outOfBoundsVec(0, 0);
	// Check Left Side
	if (objectBounds.left < areaBounds.left)
		outOfBoundsVec.x = -1;
	// Check Right Side
	if (objectBounds.left + objectBounds.width > areaBounds.left + areaBounds.width)
		outOfBoundsVec.x = 1;
	// Check Top Side
	if (objectBounds.top < areaBounds.top)
		outOfBoundsVec.y = -1;
	// Check Bottom Side
	if (objectBounds.top + objectBounds.height > areaBounds.top + areaBounds.height)
		outOfBoundsVec.y = 1;

	return outOfBoundsVec;
}

/** IsObjectWithinAreaVectorInclusive: Checks whether an object is within another one & returns a vector indicating on which side it is out of bounds (Border already == collision)
 * @return sf::Vector2f x=-1: left, x=1: right, y=-1: top, y=1: bottom, x/y=0: in bounds
*/
sf::Vector2f Level::IsObjectWithinAreaVectorInclusive(sf::FloatRect objectBounds, sf::FloatRect areaBounds) {
	sf::Vector2f outOfBoundsVec(0, 0);
	// Check Left Side
	if (objectBounds.left <= areaBounds.left)
		outOfBoundsVec.x = -1;
	// Check Right Side
	if (objectBounds.left + objectBounds.width >= areaBounds.left + areaBounds.width)
		outOfBoundsVec.x = 1;
	// Check Top Side
	if (objectBounds.top <= areaBounds.top)
		outOfBoundsVec.y = -1;
	// Check Bottom Side
	if (objectBounds.top + objectBounds.height >= areaBounds.top + areaBounds.height)
		outOfBoundsVec.y = 1;

	return outOfBoundsVec;
}

/** LoadLevelFromFile
 * Loads level data from a file, creates a Level Object of it and returns a pointer to the Level object.
 * @param file Path & filename to the file containing the level data
 * @return Level* A pointer to the loaded Level object
*/
std::shared_ptr<Level> Level::LoadLevelFromFile(std::string file) {
	std::shared_ptr<Level> level = nullptr;

	std::ifstream myfile(file); // Open filestream

	std::vector<std::vector<std::vector<std::string>>> gameObjectGrid; // Read gameObjets will be stored in here 

	std::string line; // Current reading line will be stored in here
	std::locale loc; // Locale used for checking if a char is alpha


	if (myfile.is_open()) {

		int yOffset = -1; // Initialize yOffset with -1 to read level dimensions
		int xSize = 0;
		int ySize = 0;
		bool gameObjects = false; // Defines state; If false the tile bitmap is read, true => reading gameobjects now

		std::regex txt_regex_GO_identifier("([^\(]+)"); // Regex to find game object identifier
		std::regex txt_regex_coordinates("\\(([0-9]+,[0-9]+)\\)"); // Regex to find coordinate tupels next to gameobject identifier

		while (std::getline(myfile, line)) { // Loop through each line

			if (line.length() == 0)
				continue; // Skip if line has no characters

			std::stringstream ss{ line }; // Open up a stringstream for the current line
			std::string entry;

			if (yOffset == -1) { // First iteration; Read level dimensions
				int delimCounter = 0;
				while (std::getline(ss, entry, ':')) { // Split line by delimeter and iterate through parts
					if (delimCounter == 0)
						xSize = static_cast<int>(std::stoi(entry)); // First entry = x size
					else if (delimCounter == 1)
						ySize = static_cast<int>(std::stoi(entry)); // Second entry = y size

					delimCounter++;
				}

				yOffset = 0; // Set yOffset to 0 to start reading bitmap in next iteration

				// Initialize grid vectors
				gameObjectGrid = std::vector<std::vector<std::vector<std::string>>>(ySize, std::vector<std::vector<std::string>>(xSize, std::vector <std::string>(0)));
			}
			else {

				if (line.length() > 0 && std::isalpha(line[0], loc) && !gameObjects) {
					// End of Tile Grid -> Switch mode to GameObject parsing!
					gameObjects = true;
					yOffset = 0; // Reset yOffset
				}

				if (yOffset >= ySize)
					break; // Exit if the offset is larger than the actual size

				int xOffset = 0; // Initialize current x offset


				if (!gameObjects) { // Mode: Read Tile Bitmap
					// Iterate through each character of the current line
					for (int i = 0; i < line.length(); i++) {
						std::string goPrefabType;

						// Check if current char isn't a valid tile type ID
						if (!GameObjectPrefab::IsValidGOPrefabChar(std::string(1, line[i]))) {
							goPrefabType = GameObjectPrefab::defaultGameObjectPrefabChar; // Set default tiletype because read tiletype ID is unknown
						}
						else {
							goPrefabType = line[i];
						}

						if (xOffset < xSize) {
							//gameObjectGrid[yOffset][xOffset] = goPrefabType; // Write tiletype to grid
							gameObjectGrid[yOffset][xOffset].push_back(goPrefabType);
						}
						xOffset++; // Increase xOffset
					}
				}
				else { // Mode: Read GameObjects

					// Match GameObject Identifier
					std::smatch identifierMatches;
					std::string identifier = "";
					if (std::regex_search(line, identifierMatches, txt_regex_GO_identifier)) {
						identifier = identifierMatches[0]; // Store current identifier
					}

					// Check if found identifier is a valid game object identifier
					bool isValidIdentifier = GameObjectPrefab::IsValidGOPrefabChar(identifier);

					// Match GameObject coordinates
					std::smatch coordinateMatches;
					// Read gameobject coordinates while there are more tupels
					while (std::regex_search(line, coordinateMatches, txt_regex_coordinates)) {
						if (coordinateMatches.size() >= 2 && isValidIdentifier) {
							// Split found result x,y by delimeter ','
							std::vector<std::string> coords = SplitStringByDeli(coordinateMatches[1], ',');
							if (coords.size() >= 2) {
								int x = std::stoi(coords[0]);
								int y = std::stoi(coords[1]);
								//gameObjectGrid[y][x] = identifier; // Write gameobject type to coordinate
								gameObjectGrid[y][x].push_back(identifier); // Write gameobject type to coordinate
							}
						}
						line = coordinateMatches.suffix().str();
					}
				}

				yOffset++; // Increase yOffset for next iteration
			}
		}

		if (yOffset > 1) {
			level = std::make_shared<Level>(gameObjectGrid, Level::defaultTileSize, "Level");
			level->levelFile = file;
			//level = new Level(gameObjectGrid, Level::defaultTileSize, "Level"); // Create level with found data
			level->levelFile = file;
		}

	}
	myfile.close();

	return level;
}

void Level::DrawLevel(sf::RenderWindow &rw, sf::View view) {
	rw.setView(view);
	for (auto & layerEntries : gameObjectsDrawLayer) {
		if (layerEntries.second.size() > 0) {
			for (std::shared_ptr<GameObject> go : layerEntries.second) {
				//rw.draw(go->collider); // For Debug
				if (go != nullptr) {
					rw.draw(*go->sprite);//(go->prefab->isAnimatedSprite?*go->GetAnimatedSprite : *go->GetSprite())
				}
					
			}
		}
		
	}


}

void Level::SpawnPlayer(std::shared_ptr<sf::View> playerCamera) {
	if (playerObject != nullptr) {
		playerObject->SetPosition(playerSpawnLocation);
		playerObject->playerCamera = playerCamera;
		AddGameObject(playerObject);
		return;
	}

	playerObject = std::make_shared<PlayerObject>();
	playerObject->playerCamera = playerCamera;
	playerObject->prefab = GameObjectPrefab::gameObjectPrefabs["Player"];
	playerObject->sprite->setTexture(*GameObjectPrefab::gameObjectPrefabTextures["Player"]);
	playerObject->SetPosition(playerSpawnLocation);
	playerObject->isSolid = true;
	playerObject->tag = "Player";
	playerObject->renderLayer = 3;

	AddGameObject(playerObject);
	/*
	playerObject->Start();

	gameObjects.push_back(playerObject);
	movableObjects.push_back(playerObject);
	playerObject->renderLayer = 3;
	if (gameObjectsDrawLayer.find(3) == gameObjectsDrawLayer.end()) {
		gameObjectsDrawLayer.insert({ 3,std::vector<std::shared_ptr<GameObject>>(1,playerObject) });
	}
	else {
		gameObjectsDrawLayer[3].push_back(playerObject);
	}*/

	this->playerObject = playerObject;
}

void Level::Update() {
	deltaTime = clock.restart().asSeconds();
	//std::cout << deltaTime << std::endl;
	/*
	// Reset Collision Storage of last frame
	for (std::shared_ptr<MovableObject> go : movableObjects) {
		go->collisions.clear();
		go->solidHorCollisionSides = sf::Vector2f(0, 0);
	}*/

	// Checks if a movable object collided with any object
	if (collisionCheckCooldownCounter <= 0) {
		DoCollisionChecks();
		//collisionCheckCooldownCounter = 0.05;
	}
	else
		collisionCheckCooldownCounter -= deltaTime;

	// Calls the Update() Method of each gameobject
	for (std::shared_ptr<GameObject> go : gameObjects) {
		if (go == nullptr)
			continue;

		go->Update();

		
		// Check if the sprite of the gameobject is an animated sprite
		if (typeid(*go->sprite) == typeid(AnimatedSprite)) {
			AnimatedSprite * animSprite = (AnimatedSprite*)(go->sprite);
			animSprite->Update();
		}
			
		// Check if object is out of bounds
		if (typeid(*go) == typeid(MovableObject)) {
			// TODO CHeck if its out of bounds NOT WORKING
			sf::Vector2f outOfBounds = Level::IsObjectWithinAreaVectorInclusive(go->collider.getGlobalBounds(), sf::FloatRect(0, 0, gridSize.x*tileSize, gridSize.y * tileSize));
			if (outOfBounds.x != 0 || outOfBounds.y != 0) {

				std::shared_ptr<MovableObject> mo = std::dynamic_pointer_cast<MovableObject>(go);
				mo->Destroy();
				/*
				sf::Vector2f newPos = mo->GetPosition();

				if (outOfBounds.x < 0) {
					newPos.x = 0; // TODO Ugly
				}
				else if (outOfBounds.x > 0) {
					newPos.x = gridSize.x*tileSize; // TODO Ugly
				}
				if (outOfBounds.y < 0) {
					newPos.y = 0; // TODO Ugly
				}
				else if (outOfBounds.y > 0) {
					newPos.y = gridSize.y * tileSize; // TODO Ugly
				}

				mo->SetPosition(newPos);*/
			}
		}
	}
}

void Level::AddGameObject(std::shared_ptr<GameObject> gameObject) {
	gameObject->Start();

	gameObjects.push_back(gameObject);

	if (typeid(*gameObject) == typeid(MovableObject) || typeid(*gameObject) == typeid(PlayerObject) || typeid(*gameObject) == typeid(EnemyObject)) {
		movableObjects.push_back(std::dynamic_pointer_cast<MovableObject>(gameObject));
	}

	// Create a function which deletes the object from the level when its requested to be destroyed
	auto deleteObjFunc = [this, gameObject]() {
		RemoveGameObject(gameObject);
	};
	gameObject->OnRequestDestroy.push_back(deleteObjFunc);

	if (gameObjectsDrawLayer.find(gameObject->renderLayer) == gameObjectsDrawLayer.end()) {
		gameObjectsDrawLayer.insert({ gameObject->renderLayer,std::vector<std::shared_ptr<GameObject>>(1,gameObject) });
	}
	else {
		gameObjectsDrawLayer[gameObject->renderLayer].push_back(gameObject);
	}
}

void Level::UnloadLevel() {
	if (gameObjects.size() > 0) {
		for (std::shared_ptr<GameObject> go : gameObjects) {
			if(go != nullptr)
				go.reset();
		}
	}

	if (movableObjects.size() > 0) {
		for (std::shared_ptr<MovableObject> go : movableObjects) {
			if (go != nullptr)
				go.reset();
		}
	}

	if (&gameObjectsDrawLayer != nullptr && gameObjectsDrawLayer.size() > 0) {
		for (auto const& entry : gameObjectsDrawLayer)
		{
			if (entry.second.size() > 0) {
				for (std::shared_ptr<GameObject> go : entry.second) {
					if (go != nullptr)
						go.reset();
				}
			}
		}
	}

	playerObject = nullptr;

	if (instance != nullptr)
		instance.reset();
}

/** SplitStringByDeli
 * Splits a string by given delimeter
 * @param stringToSplit The string to be split
 * @param delimeter The delimeter to be splitten by
 * @return std::vector<std::string> Vector with each part string
*/
std::vector<std::string> SplitStringByDeli(std::string stringToSplit, char delimeter) {
	std::vector<std::string> parts;

	std::stringstream ss{ stringToSplit };
	std::string entry;

	while (std::getline(ss, entry, delimeter)) {
		parts.push_back(entry);
	}

	return parts;
}
