#pragma once
#ifndef SFML_GRAPHICS_HPP
#include <SFML/Graphics.hpp>
#endif // !_SFML_GRAPHICS_
#ifndef GameObject
#include "GameObject.h"
#endif // !GameObject
#ifndef GameObjectPrefab
#include "GameObjectPrefab.h"
#endif // !GameObjectPrefab
#ifndef MovableObject
#include "MovableObject.h"
#endif // !MovableObject
#ifndef PlayerObject
#include "PlayerObject.h"
#endif // !PlayerObject
class Level
{
	std::vector<std::vector<std::vector<std::string>>> rawGrid; // Grid with tiletypes' identifiers (as stored in a textfile f.ex.). Used to build tileGrid
	std::vector<std::vector<std::vector<std::shared_ptr<GameObject>>>> gameObjectGrid; // Grid with GameObjects. This is used to display the grid.

	/** InitializeGameObjectGrid()
	 * Initializes the gameObjectGrid.
	*/
	void InitializeGameObjectGrid();
public:
	static const float defaultTileSize; // The default size of a tile if not specified differently
	sf::Vector2i gridSize; // Defines the size of the grid (Amount of Tiles in x/y direction)
	float tileSize; // The size of the tiles

	std::string levelName; // Name of the level
	std::string levelFile; // path & filename of level on the filesystem


	// TODO needed? std::string saveFile; // path & filename of the level save file on the filesystem


	std::vector<std::shared_ptr<GameObject>> gameObjects;
	std::vector<std::shared_ptr<MovableObject>> movableObjects;

	std::shared_ptr<PlayerObject> playerObject;
	sf::Vector2f playerSpawnLocation = sf::Vector2f(0,0);

	Level(std::vector<std::vector<std::vector<std::string>>> pRawGrid, float pTileSize, std::string pLevelName);

	Level();
	~Level();

	/** GetGameObjectsAtCoord()
	 * @return std::vector<std::shared_ptr<GameObject>> List with pointers to gameobjects at given coords
	*/
	std::vector<std::shared_ptr<GameObject>> GetGameObjectsAtCoord(unsigned int x, unsigned int y);

	void AddGameObjectAtCoord(unsigned int x, unsigned int y, GameObjectPrefab prefab);

	void DoCollisionChecks();

	sf::Vector2f GetCollisionSideVector(sf::FloatRect objectOne, sf::FloatRect collider);

	static std::shared_ptr<Level> LoadLevelFromFile(std::string file);

	void DrawLevel(sf::RenderWindow &rw, sf::View view);

	void SpawnPlayer(std::shared_ptr<sf::View> playerCamera);
};

