#pragma once

#ifndef SFML_GRAPHICS_HPP
#include <SFML/Graphics.hpp>
#endif // !_SFML_GRAPHICS_

#ifndef GameObject
#include "GameObject.h"
#endif // !GameObject
#ifndef Collision
#include "Collision.h"
#endif // !Collision

class MovableObject : public GameObject
{
public:
	float gravity = 400;
	float groundFriction = 0.009;
	float airFriction = 0.0001;

	float groundSpeed = 40;
	float airSpeed = 10;

	float jumpForce = -10;
	float maxJumpForce = -300;
	float accumulatedJumpForce = 0;
	float jumpForceMultiplier = 1;

	sf::Vector2f playerMovement;

	bool inputJump = false;
	bool inputLeft = false;
	bool inputRight = false;

	bool grounded = false;

	float jumpCooldown = 0;

	MovableObject();
	~MovableObject();

	void Update() override;
	void OnKeyUp(sf::Keyboard::Key key) override;
	void OnKeyDown(sf::Keyboard::Key key) override;
	void OnCollisionEnter(std::shared_ptr<Collision> collider) override;
};

