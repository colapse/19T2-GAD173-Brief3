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

const float Level::defaultTileSize = 32;

std::vector<std::string> SplitStringByDeli(std::string stringToSplit, char delimeter);// TODO

Level::Level(std::vector<std::vector<std::vector<std::string>>> pRawGrid, float pTileSize, std::string pLevelName) : rawGrid(pRawGrid), tileSize(pTileSize), levelName(pLevelName) {
	gridSize.y = pRawGrid.size();
	if (gridSize.y > 0)
		gridSize.x = pRawGrid[0].size();
	InitializeGameObjectGrid();
}

Level::Level()
{
}


Level::~Level()
{
}

void Level::InitializeGameObjectGrid() {
	gameObjectGrid = std::vector<std::vector<std::vector<std::shared_ptr<GameObject>>>>(gridSize.y, std::vector<std::vector<std::shared_ptr<GameObject>>>(gridSize.x, std::vector<std::shared_ptr<GameObject>>(0,nullptr)));
	gameObjects = std::vector<std::shared_ptr<GameObject>>(0, nullptr);

	for (int y = 0; y < rawGrid.size(); y++) {
		for (int x = 0; x < rawGrid[y].size(); x++) {
			for (std::string s : rawGrid[y][x]) {
				if (GameObjectPrefab::IsValidGOPrefabChar(s)) {
					// TODO: Check if is GameObject or MovableObject!

					std::shared_ptr<GameObject> gameobj_ptr = std::make_shared<GameObject>(tileSize, tileSize);
					//gameobj_ptr->sprite = sf::Sprite(*GameObjectPrefab::gameObjectPrefabTextures[s]);
					gameobj_ptr->prefab = GameObjectPrefab::gameObjectPrefabs[s];
					gameobj_ptr->collider.setTexture(GameObjectPrefab::gameObjectPrefabTextures[s]);
					gameobj_ptr->collider.setPosition(x*tileSize, y*tileSize);

					gameObjectGrid[y][x].push_back(gameobj_ptr);
					gameObjects.push_back(gameobj_ptr);

					// TodO if movable object... add to movableobjects list... 
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
	for (std::shared_ptr<MovableObject> movableObject : movableObjects) {
		for (std::shared_ptr<GameObject> gameObject : gameObjects) {
			if (gameObject != movableObject) {
				sf::Vector2f collisions = GetCollisionSideVector(movableObject->collider.getGlobalBounds(), gameObject->collider.getGlobalBounds());
				
				
				if (collisions.x != 0 && collisions.y != 0) {
					// Notify Collision for movable object
					auto movCol = std::make_shared<Collision>();
					movCol->colliderObject = gameObject;
					movCol->collisionSides = collisions;
					movableObject->OnCollisionEnter(movCol);

					// Invert collision side vector
					sf::Vector2f collisionsInverted = collisions;
					collisionsInverted.x *= -1;
					collisionsInverted.y *= -1;

					// Notify Collision for collider gameobject object
					auto goCol = std::make_shared<Collision>();
					goCol->colliderObject = movableObject;
					goCol->collisionSides = collisionsInverted;
					gameObject->OnCollisionEnter(goCol);
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
						if (!GameObjectPrefab::IsValidGOPrefabChar(""+line[i])) {
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
			//level = new Level(gameObjectGrid, Level::defaultTileSize, "Level"); // Create level with found data
			level->levelFile = file;
		}

	}
	myfile.close();

	return level;
}

void Level::DrawLevel(sf::RenderWindow &rw) {
	
	for (std::shared_ptr<GameObject> go : gameObjects) {
		rw.draw(go->collider);
	}
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
