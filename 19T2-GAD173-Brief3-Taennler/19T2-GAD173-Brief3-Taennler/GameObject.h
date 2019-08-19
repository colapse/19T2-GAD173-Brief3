#pragma once
#ifndef SFML_GRAPHICS_HPP
#include <SFML/Graphics.hpp>
#endif // !_SFML_GRAPHICS_
#ifndef Collision
#include "Collision.h"
#endif // !Collision
#ifndef GameObjectPrefab
#include "GameObjectPrefab.h"
#endif // !GameObjectPrefab
#ifndef AnimatedSprite
#include "AnimatedSprite.h"
#endif // !AnimatedSprite
#ifndef functional
#include <functional>
#endif // !functional
class Collision;

/**
 * Implementation of GameObject class
 *
 * A gameobject represents any "active" object in the game. It's Update() Method will be called every frame.
*/
class GameObject
{
protected:
	sf::Vector2f pos; // The current position of the gameobject
public:
	std::string tag = ""; // A tag can be used to identify a gameobject
	std::vector<std::function<void()>> OnRequestDestroy; // Stores functions that will be called when the gameobject requests to be destroyed/removed
	sf::RectangleShape collider; // The rectangular collider of the object
	sf::Sprite * sprite; // Pointer to the sprite representing this gameobject
	bool isSolid = true; // State defines if the gameobject is solid or not (If solid, it can collide with objects; If not, it can be used as a trigger)

	GameObjectPrefab * prefab; // Reference to the prefab type (Ugly solution)

	GameObject(float width, float height);
	GameObject();
	~GameObject();

	int renderLayer = 0; // The z-layer in which it will be drawn

	virtual void SetPosition(sf::Vector2f pos);
	sf::Vector2f GetPosition();

	virtual void Start();
	virtual void Update();
	virtual void OnKeyUp(sf::Keyboard::Key key);
	virtual void OnKeyDown(sf::Keyboard::Key key);

	virtual void OnCollisionEnter(std::shared_ptr<Collision> collider);
	virtual void OnTriggerEnter(std::shared_ptr<Collision> collider);
	virtual void Destroy();
};

