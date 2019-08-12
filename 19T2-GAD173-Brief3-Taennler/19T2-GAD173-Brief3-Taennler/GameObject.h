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
#ifndef functional
#include <functional>
#endif // !functional
class Collision;
class GameObject
{
protected:
	sf::Vector2f pos;
public:
	std::vector<std::function<void()>> OnRequestDestroy;
	sf::RectangleShape collider;
	sf::Sprite sprite; // TODO MAYBE NOT NEEDED!
	bool isSolid = true;

	GameObjectPrefab * prefab;

	GameObject(float width, float height);
	GameObject();
	~GameObject();

	int renderLayer = 0;

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

