#pragma once
#ifndef SFML_GRAPHICS_HPP
#include <SFML/Graphics.hpp>
#endif // !_SFML_GRAPHICS_
#ifndef GameObject
#include "GameObject.h"
#endif // !GameObject
class GameObject;
class Collision
{
public:
	sf::Vector2f collisionSides;
	std::shared_ptr<GameObject> colliderObject = nullptr;

	Collision();
	~Collision();
};

