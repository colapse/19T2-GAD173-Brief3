#include "pch.h"
#include "GameObjectPrefab.h"


std::map<std::string, GameObjectPrefab*> GameObjectPrefab::gameObjectPrefabs;
std::map<std::string, sf::Texture*> GameObjectPrefab::gameObjectPrefabTextures;
const std::string GameObjectPrefab::defaultGameObjectPrefabChar = "0";

bool GameObjectPrefab::IsValidGOPrefabChar(std::string s) {
	// Check if an entry with given string exists.
	std::map<std::string, GameObjectPrefab*>::iterator it = gameObjectPrefabs.find(s);
	if (it != gameObjectPrefabs.end())
	{
		return true;
	}
	return false;
}

GameObjectPrefab::GameObjectPrefab(std::string pGameObjectId, std::string pName, std::string gameObjectSprite) : gameObjectId(pGameObjectId), name(pName), spriteLoc(gameObjectSprite) {

}

GameObjectPrefab::GameObjectPrefab() : gameObjectId(""), name(" "), spriteLoc(" ")
{
}


GameObjectPrefab::~GameObjectPrefab()
{
}