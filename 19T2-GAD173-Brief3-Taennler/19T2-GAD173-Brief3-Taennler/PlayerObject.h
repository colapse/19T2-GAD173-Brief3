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

	PlayerObject(float width, float height);
	PlayerObject();
	~PlayerObject();

	void Update() override;
	void SetPosition(sf::Vector2f pos) override;
	void OnKeyUp(sf::Keyboard::Key key) override;
	void OnKeyDown(sf::Keyboard::Key key) override;
	virtual void Move(sf::Vector2f movementVector);
};

