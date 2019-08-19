#pragma once
#ifndef SFML_GRAPHICS_HPP
#include <SFML/Graphics.hpp>
#endif // !_SFML_GRAPHICS_
#ifndef GameObject
#include "GameObject.h"
#endif // !GameObject

class GameObject; // Temporarily define GameObject class (To avoid cross-reference errors in compiler)

/**
 * Implementation of Collision class
 *
 * Storage object to store information about a collision between 2 objects.
*/
class Collision
{
public:
	sf::Vector2f collisionSides; // The side in which the collision occured
	std::shared_ptr<GameObject> colliderObject = nullptr; // Reference to the collider gameobject

	Collision();
	~Collision();
};

