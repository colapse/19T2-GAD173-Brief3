// 19T2-GAD173-Brief3-Taennler.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include "GameObject.h"
#include "MovableObject.h"
#include "Collision.h"
#include "GameObjectPrefab.h"
#include "Level.h"
#include <filesystem>


void LoadTileTextures(int tileWidth, int tileHeight);
void GameLoop();

sf::Vector2i windowSize(800, 600);
sf::RenderWindow window(sf::VideoMode(windowSize.x, windowSize.y), "SFML works!");
static float deltaTime = 0;


std::string exeDir = "";
std::string fontName = "Radiant.ttf"; // Font retrieved from https://www.dafont.com/
std::string levelFolder = "levels";
std::string levelExt = ".lvl";

std::string texturePath = "textures\\";

std::shared_ptr<Level> activeLevel = nullptr;

sf::View mainMenu;
sf::View game;

int main()
{
	// Define current filesystem path
	exeDir = std::filesystem::current_path().u8string() + "\\";
	// Complete texturePath
	texturePath = exeDir + texturePath;


	// Initialize Gameobject Types
	GameObjectPrefab::gameObjectPrefabs = {
		{"0",new GameObjectPrefab("0", "Air", texturePath + "BlockSky.PNG")},
		{"1",new GameObjectPrefab("1", "Ground", texturePath + "BlockPlatform.PNG")},
		{"2",new GameObjectPrefab("2", "Lava", texturePath + "LavaAnimated.GIF")},
		{"Coin",new GameObjectPrefab("Coin", "Coin", texturePath + "CoinAnimated.GIF")},
		{"Enemy",new GameObjectPrefab("Enemy", "Enemy Spawn", texturePath + "EnemyAlive.PNG")},
		{"Player",new GameObjectPrefab("Player", "Player Spawn", texturePath + "Player.PNG")},
		{"Exit",new GameObjectPrefab("Exit", "Player Exit", texturePath + "Door.PNG")}
	};

	LoadTileTextures(32, 32); // Loads the tile/gameobject textures with size 32x32px

	// TODO INITIALIZE VIEWS

	activeLevel = Level::LoadLevelFromFile(exeDir+levelFolder+"\\Test.lvl"); // TMP TEST TODO

	// Contains the application/game loop
	GameLoop();
}

void GameLoop() {
	sf::Clock clock;
	GameObject go;
	MovableObject ch;

	std::vector<sf::Keyboard::Key> keyReleases;
	std::vector<sf::Keyboard::Key> keyPresses;
	while (window.isOpen())
	{
		deltaTime = clock.restart().asSeconds();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			switch (event.type) {
			case sf::Event::KeyReleased:
				keyReleases.push_back(event.key.code);
				/*
				if (event.key.code == sf::Keyboard::Space) {
					//inputJump = false;
				}
				if (event.key.code == sf::Keyboard::Left) {
					//inputLeft = false;
				}

				if (event.key.code == sf::Keyboard::Right) {
					//inputRight = false;
				}*/
				break;
			case sf::Event::KeyPressed:
				keyPresses.push_back(event.key.code);/*
				if (event.key.code == sf::Keyboard::Space) {
					//inputJump = true;
				}
				if (event.key.code == sf::Keyboard::Left) {
					//inputLeft = true;
				}

				if (event.key.code == sf::Keyboard::Right) {
					//inputRight = true;
				}*/
				break;
			}
		}

		window.clear();

		for (sf::Keyboard::Key key : keyPresses) {
			go.OnKeyDown(key);
			ch.OnKeyDown(key);
		}
		for (sf::Keyboard::Key key : keyReleases) {
			go.OnKeyUp(key);
			ch.OnKeyUp(key);
		}
		go.Update();
		ch.Update();

		//std::cout << "Character: " + std::to_string(ch.GetPosition().x) << std::endl;
		//std::cout << "GameObject: " + std::to_string(go.GetPosition().x) << std::endl;

		if (activeLevel != nullptr) {
			activeLevel->DrawLevel(window);
		}

		window.display();

		// Empty Key presses/releases lists
		keyPresses.clear();
		keyReleases.clear();
	}
}

/** LoadTileTextures
 * Loads all tile & gameobject textures
*/
void LoadTileTextures(int tileWidth, int tileHeight) {
	//tileTypeTextures = std::map<char, sf::Texture>(tileTypes.size());
	for (auto itr = GameObjectPrefab::gameObjectPrefabs.begin(); itr != GameObjectPrefab::gameObjectPrefabs.end(); ++itr) {
		sf::Texture * texture = new sf::Texture();
		if (!texture->loadFromFile(itr->second->spriteLoc, sf::IntRect(0, 0, tileWidth, tileHeight)))
		{
		}
		else {
			GameObjectPrefab::gameObjectPrefabTextures.insert({ itr->first,texture });
		}
	}
}