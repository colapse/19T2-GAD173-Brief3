#pragma once
#ifndef SFML_GRAPHICS_HPP
#include <SFML/Graphics.hpp>
#endif // !_SFML_GRAPHICS_
#ifndef GameObjectPrefab
#include "GameObjectPrefab.h"
#endif // !GameObjectPrefab
#ifndef GameObject
#include "GameObject.h"
#endif // !GameObject
#ifndef MovableObject
#include "MovableObject.h"
#endif // !MovableObject
#ifndef PlayerObject
#include "PlayerObject.h"
#endif // !PlayerObject
#ifndef EnemyObject
#include "EnemyObject.h"
#endif // !EnemyObject
class MovableObject;
class PlayerObject;
class EnemyObject;

class Level
{
	std::vector<std::vector<std::vector<std::string>>> rawGrid; // Grid with tiletypes' identifiers (as stored in a textfile f.ex.). Used to build tileGrid
	std::vector<std::vector<std::vector<std::shared_ptr<GameObject>>>> gameObjectGrid; // Grid with GameObjects. This is used to display the grid.

	/** InitializeGameObjectGrid()
	 * Initializes the gameObjectGrid.
	*/
	void InitializeGameObjectGrid();

	float collisionCheckCooldownCounter = 0;
public:
	static float deltaTime;
	static const float defaultTileSize; // The default size of a tile if not specified differently
	static std::shared_ptr<Level> instance; // Instance of currently active level
	sf::Vector2i gridSize; // Defines the size of the grid (Amount of Tiles in x/y direction)
	float tileSize; // The size of the tiles
	bool playerReachedEnd = false; // States whether the player has reached the exit

	sf::Clock clock;

	std::string levelName; // Name of the level
	std::string levelFile; // path & filename of level on the filesystem

	std::vector<std::shared_ptr<GameObject>> gameObjects; // Stores references to all gameobjects contained in the level
	std::vector<std::shared_ptr<MovableObject>> movableObjects; // Stores references to all movableobjects (Also stored in gameObjects!)
	std::map<int, std::vector<std::shared_ptr<GameObject>>, std::less<int>> gameObjectsDrawLayer; // int = render layer; Stores all Gameobjects that need to be drawn in their specific drawlayer

	std::shared_ptr<PlayerObject> playerObject = nullptr; // Reference to the player object
	sf::Vector2f playerSpawnLocation = sf::Vector2f(0,0); // Spawn location of the player

	Level(std::vector<std::vector<std::vector<std::string>>> pRawGrid, float pTileSize, std::string pLevelName);

	Level();
	~Level();

	/** GetGameObjectsAtCoord()
	 * @return std::vector<std::shared_ptr<GameObject>> List with pointers to gameobjects at given coords
	*/
	std::vector<std::shared_ptr<GameObject>> GetGameObjectsAtCoord(unsigned int x, unsigned int y);

	void DoCollisionChecks();

	static sf::Vector2f GetCollisionSideVector(sf::FloatRect objectOne, sf::FloatRect collider);

	static sf::Vector2f GetObjectDistanceWithinAreaVector(sf::FloatRect objectBounds, sf::FloatRect areaBounds);

	static sf::Vector2f IsObjectWithinAreaVectorInclusive(sf::FloatRect objectBounds, sf::FloatRect areaBounds);

	static sf::Vector2f IsObjectWithinAreaVector(sf::FloatRect objectBounds, sf::FloatRect areaBounds);

	static std::shared_ptr<Level> LoadLevelFromFile(std::string file);

	void DrawLevel(sf::RenderWindow &rw, sf::View view);

	void SpawnPlayer(std::shared_ptr<sf::View> playerCamera);

	void Update();

	void RemoveGameObject(std::shared_ptr<GameObject> gameObject);

	void AddGameObject(std::shared_ptr<GameObject> gameObject);

	void UnloadLevel();
};

