// 19T2-GAD173-Brief3-Taennler.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Level.h"
#include "GameObject.h"
#include "MovableObject.h"
#include "PlayerObject.h"
#include "Collision.h"
#include "GameObjectPrefab.h"
#include "Button.h"
#include <filesystem>


enum class ViewName
{
	MainMenu,
	Game,
	GameHeader
};

void LoadTileTextures(int tileWidth, int tileHeight);
void GameLoop();
void BuildMainMenuView();
void BuildGameView();
void BuildGameHeaderView();
void InitVars();
void AddAndSetActiveView(ViewName viewName);
void HandleViewButtonEvents(ViewName viewName, sf::Event e);
std::vector<std::string> LoadLevelsFromDirectory();

sf::Vector2i windowSize(800, 600);
sf::RenderWindow window(sf::VideoMode(windowSize.x, windowSize.y), "Cool Game");
sf::Font generalFont;
sf::Vector2f mousePos;
sf::Vector2i mousePosInt;
static float deltaTime = 0;

std::string exeDir = "";
std::string fontName = "Radiant.ttf"; // Font retrieved from https://www.dafont.com/
std::string levelFolder = "levels";
std::string levelExt = ".lvl";

std::string texturePath = "textures\\";

std::shared_ptr<Level> activeLevel = nullptr;


std::vector<ViewName> activeViews;

std::map<ViewName, std::shared_ptr<sf::View>> views;
std::map<ViewName, std::vector<std::shared_ptr<sf::Drawable>>> viewDrawables;
std::map<ViewName, std::vector<std::shared_ptr<Button>>> viewButtons;

int main()
{
	InitVars(); // Initializes a set of critical variables

	//activeLevel = Level::LoadLevelFromFile(exeDir+levelFolder+"\\Test.lvl"); // TMP TEST TODO

	AddAndSetActiveView(ViewName::MainMenu);

	// Contains the application/game loop
	GameLoop();
}

/** InitVars
 * Initializes basic vars
*/
void InitVars() {
	// Try to load the main font
	if (!generalFont.loadFromFile(exeDir + fontName))
	{
		std::cout << "Couldn't load font!" << std::endl;
	}

	// Define current filesystem path
	exeDir = std::filesystem::current_path().u8string() + "\\";
	// Complete texturePath
	texturePath = exeDir + texturePath;


	// Initialize Gameobject Types
	GameObjectPrefab::gameObjectPrefabs = {
		{"0",new GameObjectPrefab("0", "Air", texturePath + "BlockSky.PNG", true, false)},
		{"1",new GameObjectPrefab("1", "Ground", texturePath + "BlockPlatform.PNG", true, true)},
		{"2",new GameObjectPrefab("2", "Lava", texturePath + "LavaAnimated.GIF", true, false)},
		{"Coin",new GameObjectPrefab("Coin", "Coin", texturePath + "CoinAnimated.GIF", false, false)},
		{"Enemy",new GameObjectPrefab("Enemy", "Enemy Spawn", texturePath + "EnemyAlive.PNG", false, true)},
		{"Player",new GameObjectPrefab("Player", "Player Spawn", texturePath + "Player.PNG", false, true)},
		{"Exit",new GameObjectPrefab("Exit", "Player Exit", texturePath + "Door.PNG", true, false)}
	};

	LoadTileTextures(32, 32); // Loads the tile/gameobject textures with size 32x32px

	// Initialize Views
	views.insert({ ViewName::MainMenu,std::make_shared<sf::View>(sf::Vector2f(windowSize.x / 2, windowSize.y / 2), sf::Vector2f(windowSize)) });
	views.insert({ ViewName::GameHeader,std::make_shared<sf::View>(sf::FloatRect(0, 0, windowSize.x, windowSize.y*0.1f)) });
	views[ViewName::GameHeader]->setViewport(sf::FloatRect(0, 0, 1, 0.1f));
	//views[ViewName::GameHeader]->zoom(1);
	views.insert({ ViewName::Game,std::make_shared<sf::View>(sf::FloatRect(0, 0, windowSize.x, windowSize.y*0.9f)) });
	views[ViewName::Game]->setViewport(sf::FloatRect(0, 0.1f, 1, 0.9f));
	//views[ViewName::Game]->zoom(3);

}

void AddAndSetActiveView(ViewName viewName) {

	switch (viewName) {
	case ViewName::MainMenu:
	{
		activeViews.clear();
		BuildMainMenuView();
		break;
	}
	case ViewName::Game: {
		if (std::find(activeViews.begin(), activeViews.end(), ViewName::MainMenu) != activeViews.end()) {
			activeViews.erase(std::remove(activeViews.begin(), activeViews.end(), ViewName::MainMenu), activeViews.end());
		}
		BuildGameView();
		if (activeLevel != nullptr) {
			activeLevel->SpawnPlayer(views[ViewName::Game]);
		}
		break;
	}
	case ViewName::GameHeader: {
		if (std::find(activeViews.begin(), activeViews.end(), ViewName::MainMenu) != activeViews.end()) {
			activeViews.erase(std::remove(activeViews.begin(), activeViews.end(), ViewName::MainMenu), activeViews.end());
		}
		BuildGameHeaderView();
		break;
	}
	
	}
	activeViews.push_back(viewName);
}

void GameLoop() {
	sf::Clock clock;

	//TEST OBJECTS
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
			if (activeViews.size() > 0) {
				for (auto const& entry : activeViews) {
					HandleViewButtonEvents(entry, event);
				}
			}
			

			if (event.type == sf::Event::Closed)
				window.close();

			switch (event.type) {
			case sf::Event::MouseMoved: {
				mousePosInt = sf::Mouse::getPosition(window);
				mousePos.x = (float)mousePosInt.x;
				mousePos.y = (float)mousePosInt.y;
				break;
			}
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


		//TESTCODE
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
		/*
		if (activeLevel != nullptr) {
			activeLevel->DrawLevel(window);
		}*/
		

		if (activeViews.size() > 0) {
			for (auto const& activeView : activeViews) {
				window.setView(*views[activeView]);

				if (activeView == ViewName::Game && activeLevel != nullptr) {
					Level::deltaTime = deltaTime;
					activeLevel->Update();
					for (sf::Keyboard::Key key : keyPresses) {
						for (std::shared_ptr<MovableObject> movableObject : activeLevel->movableObjects) {
							movableObject->OnKeyDown(key);
						}
					}
					for (sf::Keyboard::Key key : keyReleases) {
						for (std::shared_ptr<MovableObject> movableObject : activeLevel->movableObjects) {
							movableObject->OnKeyUp(key);
						}
					}

					activeLevel->DrawLevel(window, *views[activeView]);
				}else if (viewDrawables.find(activeView) != viewDrawables.end())
				{
					for (auto const& entry : viewDrawables[activeView]) {
						if (entry != nullptr)
							window.draw(*entry);
					}
				}
			}
		}
		

		window.display();

		// Empty Key presses/releases lists
		keyPresses.clear();
		keyReleases.clear();
	}
}

void HandleViewButtonEvents(ViewName viewName, sf::Event e) {
	if (views.find(viewName) == views.end() || viewButtons.find(viewName) == viewButtons.end() || viewButtons[viewName].size() == 0 || views[viewName] == nullptr)
		return;

	// Break if event isn't one of the following types to increase performance
	if (e.type != sf::Event::MouseMoved && e.type != sf::Event::MouseButtonPressed && e.type != sf::Event::MouseButtonReleased)
		return;

	// Set current sf::view
	window.setView(*views[viewName]);

	// Handle events for Buttons [TODO: Super inperformant!]
	for (std::shared_ptr<Button> b : viewButtons[viewName]) {
		switch (e.type) {
		case sf::Event::MouseMoved:
			if (b->isActive && b->GetGlobalBounds().contains(window.mapPixelToCoords(mousePosInt))) {
				b->SetMouseEnter();
			}
			else if (b->isActive) {
				b->SetMouseExit();
			}
			break;
		case sf::Event::MouseButtonPressed:
			if ((e.mouseButton.button == sf::Mouse::Button::Left || e.mouseButton.button == sf::Mouse::Button::Right) && b->isActive && b->GetGlobalBounds().contains(window.mapPixelToCoords(mousePosInt))) {
				b->SetButtonPressed();
			}
			break;
		case sf::Event::MouseButtonReleased:
			if ((e.mouseButton.button == sf::Mouse::Button::Left || e.mouseButton.button == sf::Mouse::Button::Right) && b->isActive && b->GetGlobalBounds().contains(window.mapPixelToCoords(mousePosInt))) {
				b->SetButtonReleased();
			}
			break;
		}
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

/** BuildMainMenuView
 * Builds the UI for the Main Menu.
*/
void BuildMainMenuView() {
	// Clear out drawables
	if (viewDrawables.find(ViewName::MainMenu) != viewDrawables.end()) {
		viewDrawables[ViewName::MainMenu].clear();
	}

	float yOffset = 10;

	// Loads a current list of levels in the level folder
	std::vector<std::string> levelsInFolder = LoadLevelsFromDirectory();  // Filenames of levels in level folder

	// Title
	std::shared_ptr<sf::Text> txtTitle = std::make_shared<sf::Text>("Cool Game", generalFont, 30);
	sf::Vector2f titlePos(views[ViewName::MainMenu]->getSize().x / 2 - txtTitle->getGlobalBounds().width / 2, yOffset);
	txtTitle->setPosition(titlePos);
	yOffset += txtTitle->getGlobalBounds().height + 30;
	viewDrawables[ViewName::MainMenu].push_back(txtTitle);

	// Txt Level Folder
	std::shared_ptr < sf::Text> txtFastAccess = std::make_shared<sf::Text>("Select a Level: ", generalFont, 18);
	sf::Vector2f txtFastAccessPos(views[ViewName::MainMenu]->getSize().x / 2 - txtFastAccess->getGlobalBounds().width / 2, yOffset);
	txtFastAccess->setPosition(txtFastAccessPos);
	viewDrawables[ViewName::MainMenu].push_back(txtFastAccess);
	yOffset += txtFastAccess->getGlobalBounds().height + 20;


	for (int i = 0; i < levelsInFolder.size(); i++) {
		float xPos = views[ViewName::MainMenu]->getSize().x / 2 - 125 + (i % 5) * 60;
		// Button Open Level
		std::shared_ptr<Button> btnLevel = std::make_shared<Button>();
		std::shared_ptr < sf::RectangleShape> rectBtnLevel = std::make_shared <sf::RectangleShape>(sf::Vector2f(50, 50));
		btnLevel->SetShape(rectBtnLevel);
		btnLevel->SetFillColor(sf::Color::Green);
		btnLevel->SetText(levelsInFolder[i], sf::Color::Black);
		btnLevel->SetFont(generalFont);
		btnLevel->SetOutline(2, sf::Color::Color(0, 143, 43, 255));
		btnLevel->SetPosition(xPos, yOffset);
		//// Create mouse enter lambda func which changes the BG col
		auto FnMOver_BTNLevel = [btnLevel]() {
			btnLevel->SetFillColor(sf::Color::Color(0, 143, 43, 255));
		};
		btnLevel->AddMouseEnterFunc(FnMOver_BTNLevel); // Assign func to input
		//// Create mouse exit lambda func which changes the BG col
		auto FnMExit_BTNLevel = [btnLevel]() {
			btnLevel->SetFillColor(sf::Color::Green);
		};
		btnLevel->AddMouseExitFunc(FnMExit_BTNLevel); // Assign func to input
		//// Create button release lambda func which changes the BG col
		std::string filename = levelsInFolder[i];
		auto FnMRelease_BTNLevel = [filename]() {
			// TODO Load Level
			std::cout << "[TODO] Load Level " + filename + "!" << std::endl;
			std::shared_ptr<Level> level = Level::LoadLevelFromFile(exeDir + levelFolder + "\\" + filename + levelExt);

			if (level != nullptr) {
				activeLevel = level;
				AddAndSetActiveView(ViewName::Game);
				AddAndSetActiveView(ViewName::GameHeader);
			}
			else {
				std::cout << "Couldn't load level " + filename << std::endl;
			}
		};
		btnLevel->AddButtonReleasedFunc(FnMRelease_BTNLevel); // Assign func to input
		viewDrawables[ViewName::MainMenu].push_back(btnLevel->GetShapeObject());
		viewDrawables[ViewName::MainMenu].push_back(btnLevel->GetSpriteObject());
		viewDrawables[ViewName::MainMenu].push_back(btnLevel->GetTextObject());
		viewButtons[ViewName::MainMenu].push_back(btnLevel);
		if ((i + 1) % 5 == 0)
			yOffset += btnLevel->GetGlobalBounds().height + 10;
	}
}

/** BuildGameView
 * Builds the GameView / Active Level.
*/
void BuildGameView() {

}

/** BuildGameView
 * Builds the GameView / Active Level.
*/
void BuildGameHeaderView() {
	float yOffset = 10;
	float xOffset = 10;

	//Background
	std::shared_ptr<sf::RectangleShape> backgroundRect = std::make_shared< sf::RectangleShape>(sf::Vector2f(windowSize.x,windowSize.y*0.1f));
	backgroundRect->setFillColor(sf::Color::Cyan);
	viewDrawables[ViewName::GameHeader].push_back(backgroundRect);

	// Button MainMenu
	std::shared_ptr<Button> btnMainMenu = std::make_shared<Button>();
	std::shared_ptr < sf::RectangleShape> rectBtnMainMenu = std::make_shared <sf::RectangleShape>(sf::Vector2f(50, 30));
	btnMainMenu->SetShape(rectBtnMainMenu);
	btnMainMenu->SetFillColor(sf::Color::Green);
	btnMainMenu->SetText("QUIT", sf::Color::Black);
	btnMainMenu->SetFont(generalFont);
	btnMainMenu->SetOutline(2, sf::Color::Color(0, 143, 43, 255));
	btnMainMenu->SetPosition(xOffset, yOffset);
	//// Create mouse enter lambda func which changes the BG col
	auto FnMOver_BTNLevel = [btnMainMenu]() {
		btnMainMenu->SetFillColor(sf::Color::Color(0, 143, 43, 255));
	};
	btnMainMenu->AddMouseEnterFunc(FnMOver_BTNLevel); // Assign func to input
	//// Create mouse exit lambda func which changes the BG col
	auto FnMExit_BTNLevel = [btnMainMenu]() {
		btnMainMenu->SetFillColor(sf::Color::Green);
	};
	btnMainMenu->AddMouseExitFunc(FnMExit_BTNLevel); // Assign func to input
	//// Create button release lambda func which changes the BG col
	auto FnMRelease_BTNLevel = []() {
		activeLevel = nullptr;
		AddAndSetActiveView(ViewName::MainMenu);
	};
	btnMainMenu->AddButtonReleasedFunc(FnMRelease_BTNLevel); // Assign func to input
	viewDrawables[ViewName::GameHeader].push_back(btnMainMenu->GetShapeObject());
	viewDrawables[ViewName::GameHeader].push_back(btnMainMenu->GetSpriteObject());
	viewDrawables[ViewName::GameHeader].push_back(btnMainMenu->GetTextObject());
	viewButtons[ViewName::GameHeader].push_back(btnMainMenu);
}

/** LoadLevelsFromDirectory
 * Read all levels from level directory
 * original from https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
*/
std::vector<std::string> LoadLevelsFromDirectory() {
	std::vector<std::string> levelsInFolder;

	// Iterate through level folder files
	for (const auto & entry : std::filesystem::directory_iterator(exeDir + levelFolder)) {
		// Check if current file has the levelExt extension
		if (entry.is_regular_file() && entry.path().extension() == levelExt) {
			levelsInFolder.push_back(entry.path().stem().u8string()); // Add filename to list
		}
	}

	return levelsInFolder;
}