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

const float Level::defaultTileSize = 32;
float Level::deltaTime = 0.01f;
std::shared_ptr<Level> Level::instance = nullptr;

std::vector<std::string> SplitStringByDeli(std::string stringToSplit, char delimeter); // Method not part of class! (Ugly)

Level::Level(std::vector<std::vector<std::vector<std::string>>> pRawGrid, float pTileSize, std::string pLevelName) : rawGrid(pRawGrid), tileSize(pTileSize), levelName(pLevelName) {
	deltaTime = 0.01f;
	gridSize.y = (int)pRawGrid.size();
	if (gridSize.y > 0)
		gridSize.x = (int)pRawGrid[0].size();

	InitializeGameObjectGrid(); // Creates gameobjects and positions them depending on the given grid
}

Level::Level()
{
	deltaTime = 0.01f;
}

Level::~Level()
{
	deltaTime = 0.01f;
}

/** Method RemoveGameObject(): Removes a gameobject from the level */
void Level::RemoveGameObject(std::shared_ptr<GameObject> gameObject) {
	if (gameObject == nullptr)
		return;
	
	try {
		// Remove gameobject from gameObjects container
		if (gameObjects.size() > 0 && std::find(gameObjects.begin(), gameObjects.end(), gameObject) != gameObjects.end())
			gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), gameObject), gameObjects.end());

		// Check if given gameObject is a MovableObject. Remove it from the list.
		if (movableObjects.size() > 0 && typeid(*gameObject) == typeid(MovableObject)) {
			std::shared_ptr<MovableObject> moveableObject = std::dynamic_pointer_cast<MovableObject>(gameObject);
			if (moveableObject != nullptr && std::find(movableObjects.begin(), movableObjects.end(), moveableObject) != movableObjects.end())
				movableObjects.erase(std::remove(movableObjects.begin(), movableObjects.end(), moveableObject), movableObjects.end());
		}

		// Remove gameobject from the drawlayer container
		if (gameObjectsDrawLayer.size() > 0 && gameObjectsDrawLayer.find(gameObject->renderLayer) != gameObjectsDrawLayer.end() && gameObjectsDrawLayer[gameObject->renderLayer].size() > 0 && std::find(gameObjectsDrawLayer[gameObject->renderLayer].begin(), gameObjectsDrawLayer[gameObject->renderLayer].end(), gameObject) != gameObjectsDrawLayer[gameObject->renderLayer].end())
			gameObjectsDrawLayer[gameObject->renderLayer].erase(std::remove(gameObjectsDrawLayer[gameObject->renderLayer].begin(), gameObjectsDrawLayer[gameObject->renderLayer].end(), gameObject), gameObjectsDrawLayer[gameObject->renderLayer].end());
	}
	catch (const std::exception& e) {

	}
	
}

/** Method InitializeGameObjectGrid(): Creates GameObjects from the grid loaded from the level file. */
void Level::InitializeGameObjectGrid() {
	gameObjectGrid = std::vector<std::vector<std::vector<std::shared_ptr<GameObject>>>>(gridSize.y, std::vector<std::vector<std::shared_ptr<GameObject>>>(gridSize.x, std::vector<std::shared_ptr<GameObject>>(0,nullptr)));
	gameObjects = std::vector<std::shared_ptr<GameObject>>(0, nullptr);

	for (int y = 0; y < rawGrid.size(); y++) {
		for (int x = 0; x < rawGrid[y].size(); x++) {
			for (std::string s : rawGrid[y][x]) {
				if (GameObjectPrefab::IsValidGOPrefabChar(s)) {
					// Differentiate between Gameobject sub-types
					if (GameObjectPrefab::gameObjectPrefabs[s]->isStatic) {// Simple GameObject, can't move
						// Create new Gameobject
						std::shared_ptr<GameObject> gameobj_ptr = std::make_shared<GameObject>(tileSize, tileSize);
						gameobj_ptr->prefab = GameObjectPrefab::gameObjectPrefabs[s];
						
						// Check if this gameobject type has an animated sprite
						if (GameObjectPrefab::gameObjectPrefabs[s]->isAnimatedSprite) {
							AnimatedSprite * animSprite = new AnimatedSprite();

							// Initialize settings of animated sprite depending on its type
							if (s == "2") { // Lava
								animSprite->frameCountX = 5;
								animSprite->frameCountY = 1;
								animSprite->tileSize = tileSize;
								animSprite->fps = 10;
							}
							else if (s == "Coin") { // Coin
								animSprite->frameCountX = 5;
								animSprite->frameCountY = 4;
								animSprite->tileSize = tileSize;
								animSprite->fps = 20;
							}

							// Delete old sprite if there is one
							if (gameobj_ptr->sprite != nullptr)
								delete gameobj_ptr->sprite;

							// Assign point to animated sprite to the gameobject
							gameobj_ptr->sprite = animSprite;
						}

						gameobj_ptr->sprite->setTexture(*GameObjectPrefab::gameObjectPrefabTextures[s]);
						gameobj_ptr->SetPosition(sf::Vector2f(x*tileSize, y*tileSize));
						gameobj_ptr->isSolid = GameObjectPrefab::gameObjectPrefabs[s]->isSolid;
						gameobj_ptr->renderLayer = 0; // Static objects will be drawn all the way in the back

						// Set Tag for later identification
						gameobj_ptr->tag = s;

						// Stores a reference of the static gameobject in the gameObjectGrid
						gameObjectGrid[y][x].push_back(gameobj_ptr);

						// This method will add the gameobject to a list and also apply an OnDestroy method to the gameobject
						AddGameObject(gameobj_ptr);
					}
					else { // Non-static objects -> MovableObjects
						if (s == "Player") { // The player doesn't get spawned yet, only his spawn location is set
							playerSpawnLocation = sf::Vector2f(x*tileSize, y*tileSize);
						}
						else if (s == "Enemy") { // EnemyObject (Derivative of MovableObject)
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
						else {// Normal MovableObject
							std::shared_ptr<MovableObject> gameobj_ptr = std::make_shared<MovableObject>(tileSize, tileSize);

							gameobj_ptr->prefab = GameObjectPrefab::gameObjectPrefabs[s];

							// Check if this gameobject type has an animated sprite
							if (GameObjectPrefab::gameObjectPrefabs[s]->isAnimatedSprite) {
								AnimatedSprite * animSprite = new AnimatedSprite();
								if (s == "2") {
									animSprite->frameCountX = 5;
									animSprite->frameCountY = 1;
									animSprite->tileSize = tileSize;
									animSprite->fps = 10;
								}
								else if (s == "Coin") {
									animSprite->frameCountX = 5;
									animSprite->frameCountY = 4;
									animSprite->tileSize = tileSize;
									animSprite->fps = 20;
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

/** Method GetGameObjectsAtCoord(): Returns all static gameobjects at a specific location in the grid. */
std::vector<std::shared_ptr<GameObject>> Level::GetGameObjectsAtCoord(unsigned int x, unsigned int y) {
	if (gameObjectGrid.size() > y && gameObjectGrid[y].size() > x)
		return gameObjectGrid[y][x];
	return std::vector<std::shared_ptr<GameObject>>();
}

/** Method DoCollisionChecks(): Checks if there are collisions between MovableObjects and any other gameobject. */
void Level::DoCollisionChecks() {
	if (movableObjects.size() == 0)
		return;

	// Iterate through all movableobjects
	for (std::shared_ptr<MovableObject> movableObject : movableObjects) {
		if (movableObject == nullptr)
			continue;

		// Delete old collisions of movableobject
		if (movableObject->collisions.size() > 0) {
			for (std::shared_ptr<Collision> col : movableObject->collisions) {
				col.reset();
			}
		}
		movableObject->collisions.clear();

		// Reset collision side values of movableobject
		movableObject->solidHorCollisionSides = sf::Vector2f(0, 0);
		movableObject->solidVertCollisionSides = sf::Vector2f(0, 0);

		// Iterate through all gameobjects & check if there are any collisions with the current movableobject
		for (std::shared_ptr<GameObject> gameObject : gameObjects) {
			// Check if the gameobject isn't a nullpointer and it's not the current movableObject we're checking (To avoid collisions with the object itself)
			if (gameObject != nullptr && gameObject != movableObject) {
				// Check if a collision occured and from which side the it occured
				sf::Vector2f collisions = GetCollisionSideVector(movableObject->collider.getGlobalBounds(), gameObject->collider.getGlobalBounds());

				// Check if there are any collision on any side
				if (collisions.x != 0 || collisions.y != 0) {
					// Notify Collision for movable object
					auto movCol = std::make_shared<Collision>();
					movCol->colliderObject = gameObject;
					movCol->collisionSides = collisions;

					// Check if the collided object is solid or not
					if (gameObject->isSolid) { // It's solid -> classified as collision
						movableObject->OnCollisionEnter(movCol);
					}
					else { // It's not solid -> classified as a trigger
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

					// Check if the collided object is solid or not
					if (movableObject->isSolid) // It's solid -> classified as collision
						gameObject->OnCollisionEnter(goCol);
					else // It's not solid -> classified as a trigger
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

/** Method DrawLevel: Draws all gameobjects in the given renderwindow ordered by their draw layer. */
void Level::DrawLevel(sf::RenderWindow &rw, sf::View view) {
	// Make sure we draw in the game window
	rw.setView(view);

	// Iterate through all draw layers (This map is sorted by it's int-key)
	for (auto & layerEntries : gameObjectsDrawLayer) {
		if (layerEntries.second.size() > 0) {
			// Iterate through all gameobjects in the current layer
			for (std::shared_ptr<GameObject> go : layerEntries.second) {
				//rw.draw(go->collider); // For Debug
				if (go != nullptr && go->sprite != nullptr) {
					rw.draw(*go->sprite);// Draw the sprite of the gameobject
				}
			}
		}
	}
}

/** Method SpawnPlayer: Spawns & initializes the player object (Or assigns a new camera & sets its position if it already exists). */
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
	this->playerObject = playerObject;
}

/** Method Update: Gets called every frame. Updates deltatime, checks for collisions between gameobjects. */
void Level::Update() {
	deltaTime = clock.restart().asSeconds();

	// Checks if a movable object collided with any object (Has a cooldown)
	if (collisionCheckCooldownCounter <= 0) {
		DoCollisionChecks();
		collisionCheckCooldownCounter = 0.001f;
	}
	else
		collisionCheckCooldownCounter -= deltaTime;

	// Calls the Update() Method of each gameobject & animates the sprite
	for (std::shared_ptr<GameObject> go : gameObjects) {
		if (go == nullptr)
			continue;

		go->Update();

		// Check if the sprite of the gameobject is an animated sprite
		if (typeid(*go->sprite) == typeid(AnimatedSprite)) {
			AnimatedSprite * animSprite = (AnimatedSprite*)(go->sprite); // Convert sprite pointer to AnimatedSprite
			animSprite->Update(); // Update changes to next animation frame eventually
		}
			
		// Check if object is out of bounds
		if (typeid(*go) == typeid(MovableObject)) {
			// TODO Check if its out of bounds ( NOT WORKING )
			sf::Vector2f outOfBounds = Level::IsObjectWithinAreaVectorInclusive(go->collider.getGlobalBounds(), sf::FloatRect(0, 0, gridSize.x*tileSize, gridSize.y * tileSize));
			if (outOfBounds.x != 0 || outOfBounds.y != 0) {
				std::shared_ptr<MovableObject> mo = std::dynamic_pointer_cast<MovableObject>(go);
				mo->Destroy(); // Destroy the object
			}
		}
	}
}

/** Method AddGameObject: Adds the gameobject to the active "world". Calls Start method of the gameobject and adds it to the draw pipeline. */
void Level::AddGameObject(std::shared_ptr<GameObject> gameObject) {
	gameObject->Start(); // Start() may contain initializing values.

	gameObjects.push_back(gameObject);

	// Check if the gameobject is a MovableObject (Or a sub-type)
	if (typeid(*gameObject) == typeid(MovableObject) || typeid(*gameObject) == typeid(PlayerObject) || typeid(*gameObject) == typeid(EnemyObject)) {
		movableObjects.push_back(std::dynamic_pointer_cast<MovableObject>(gameObject));
	}

	// Create a function which deletes the object from the level when its requested to be destroyed
	auto deleteObjFunc = [this, gameObject]() {
		RemoveGameObject(gameObject);
	};
	gameObject->OnRequestDestroy.push_back(deleteObjFunc); // Assign Deletion Function to gameobject

	// Add the gameobject to the draw pipeline
	if (gameObjectsDrawLayer.find(gameObject->renderLayer) == gameObjectsDrawLayer.end()) {
		gameObjectsDrawLayer.insert({ gameObject->renderLayer,std::vector<std::shared_ptr<GameObject>>(1,gameObject) });
	}
	else {
		gameObjectsDrawLayer[gameObject->renderLayer].push_back(gameObject);
	}
}

/** Method UnloadLevel: Unloads a level. Resets all pointers to gameobjects & more. */
void Level::UnloadLevel() {
	if (gameObjects.size() > 0) {
		for (std::shared_ptr<GameObject> go : gameObjects) {
			if (go != nullptr) {
				go->Destroy();
				go.reset();
				go = nullptr;
			}
		}
		gameObjects.clear();
	}

	if (movableObjects.size() > 0) {
		for (std::shared_ptr<MovableObject> go : movableObjects) {
			if (go != nullptr) {
				go->Destroy();
				go.reset();
				go = nullptr;
			}
		}
		movableObjects.clear();
	}

	if (&gameObjectsDrawLayer != nullptr && gameObjectsDrawLayer.size() > 0) {
		for (auto const& entry : gameObjectsDrawLayer)
		{
			if (entry.second.size() > 0) {
				for (std::shared_ptr<GameObject> go : entry.second) {
					if (go != nullptr) {
						go->Destroy();
						go.reset();
						go = nullptr;
					}
				}
			}
		}
		gameObjectsDrawLayer.clear();
	}

	if (gameObjectGrid.size() > 0) {
		gameObjectGrid.clear();
	}
	playerObject.reset();

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
