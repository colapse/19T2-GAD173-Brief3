#pragma once

#ifndef SFML_GRAPHICS_HPP
#include <SFML/Graphics.hpp>
#endif // !_SFML_GRAPHICS_

#ifndef GameObject
#include "GameObject.h"
#endif // !GameObject

/**
 * Implementation of MovableObject class
 *
 * Derivative of GameObject. Contains behavior for movable objects. (Important: Collision checks are only executed going out from MovableObjects, not GameObject)
*/
class MovableObject : public GameObject
{
public:
	bool blockMovement = false; // If true, movement input will be ignored

	float gravity = 400; // Gravity force that pulls the object down (Positive number = down force)
	float groundFriction = 0.019; // Friction that slows down the movement while on the ground
	float airFriction = 0.001; // Friction that slows down the movement while in air

	float groundSpeed = 50; // Movement speed on the ground
	float airSpeed = 10; // Movement speed in the air

	float jumpForce = -30; // Up-force applied when jumping (Per second while jump input is active)
	float maxJumpForce = -250; // Maximum up-force that can be applied through jumping
	float accumulatedJumpForce = 0; // So far applied jumpforce
	float jumpForceMultiplier = 1; // Multiplier for applied jumpforce (To make it non-gradual)

	sf::Vector2f objectMovement; // Movement vector (Speed & direction)
	sf::Vector2f forceAddition; // Additional force that can be applied
	sf::Vector2f lookDirection; // Current facing direction

	bool inputJump = false; // States if the player/AI hit the jump button
	bool inputLeft = false; // States if the player/AI hit the move-left button
	bool inputRight = false; // States if the player/AI hit the move-right button

	bool grounded = false; // States if the object is grounded or not

	float jumpCooldown = 0; // Current cooldown on jumping

	std::vector<std::shared_ptr<Collision>> collisions; // Stores references to collisions in the current frame
	sf::Vector2f solidVertCollisionSides; // x = top, y = bottom; Each positive value represents how far in the object is within a collider
	sf::Vector2f solidHorCollisionSides; // x = left, y = right; Each positive value represents how far in the object is within a collider

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

