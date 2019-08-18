#pragma once

#ifndef SFML_GRAPHICS_HPP
#include <SFML/Graphics.hpp>
#endif // !_SFML_GRAPHICS_

#ifndef GameObject
#include "GameObject.h"
#endif // !GameObject

class MovableObject : public GameObject
{
public:
	bool blockMovement = false;

	float gravity = 400;
	float groundFriction = 0.019;
	float airFriction = 0.001;

	float groundSpeed = 50;
	float airSpeed = 10;

	float jumpForce = -100;
	float maxJumpForce = -195;
	float accumulatedJumpForce = 0;
	float jumpForceMultiplier = 1;

	sf::Vector2f objectMovement;
	sf::Vector2f forceAddition;
	sf::Vector2f lookDirection;

	bool inputJump = false;
	bool inputLeft = false;
	bool inputRight = false;

	bool grounded = false;

	float jumpCooldown = 0;

	std::vector<std::shared_ptr<Collision>> collisions;
	sf::Vector2f solidVertCollisionSides; // x = top, y = bottom
	sf::Vector2f solidHorCollisionSides; // x = left, y = right

	MovableObject(float width, float height);
	MovableObject();
	~MovableObject();

	void Update() override;
	void Destroy() override;
	void OnKeyUp(sf::Keyboard::Key key) override;
	void OnKeyDown(sf::Keyboard::Key key) override;
	void OnCollisionEnter(std::shared_ptr<Collision> collider) override;

	virtual bool IsGrounded();
	virtual void CalculateMovement();
	virtual void Move(sf::Vector2f movementVector);
	virtual void DoCollisionCheck();
	virtual void AddForce(sf::Vector2f force);
	virtual sf::Vector2f CalculateFinalMovementVector(sf::Vector2f movementVector);
};

