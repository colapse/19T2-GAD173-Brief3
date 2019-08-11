#pragma once

#ifndef SFML_GRAPHICS_HPP
#include <SFML/Graphics.hpp>
#endif // !_SFML_GRAPHICS_

#ifndef MovableObject
#include "MovableObject.h"
#endif // !MovableObject
class PlayerObject : public MovableObject
{
public:

	std::shared_ptr<sf::View> playerCamera;

	PlayerObject();
	~PlayerObject();

	void SetPosition(sf::Vector2f pos);
};

