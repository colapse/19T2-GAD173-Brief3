#pragma once
#ifndef string
#include <string>
#endif // !string
#ifndef map
#include <map>
#endif // !map
#ifndef SFML_GRAPHICS_HPP
#include <SFML/Graphics.hpp>
#endif // !SFML/Graphics.hpp

/** Class GameObjectPrefab
 * Defines some gameobject prefab such as ground, coin, player, ...
*/
class GameObjectPrefab
{
public:
	static std::map<std::string, GameObjectPrefab*> gameObjectPrefabs; // List containing pointers to all gameobject type objects (string is the unique identifier)
	static std::map<std::string, sf::Texture*> gameObjectPrefabTextures; // Pointer to textures of the gameobject types
	static const std::string defaultGameObjectPrefabChar; // string ID of the default gameobject type

	/** IsValidGOTypeChar()
	 * @param s The string-ID of the gameobject type to check.
	 * @return bool Returns true if a gamepbject type with the give string ID exists, false elsewise
	*/
	static bool IsValidGOPrefabChar(std::string s);

	const std::string gameObjectId; // string ID of the gameobejct type
	const std::string name; // Name of the gameobject
	const std::string spriteLoc; // Path + Filename of the image representing the gameobject
	const bool isStatic = true;
	const bool isSolid = true;
	bool isAnimatedSprite = false;

	GameObjectPrefab(std::string pGameObjectId, std::string pName, std::string gameObjectSprite, bool isStatic, bool isSolid, bool isAnimatedSprite);
	GameObjectPrefab();
	~GameObjectPrefab();
};

