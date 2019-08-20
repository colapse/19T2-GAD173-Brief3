#pragma once

#ifndef SFML_GRAPHICS_HPP
#include <SFML/Graphics.hpp>
#endif // !_SFML_GRAPHICS_

#ifndef MovableObject
#include "MovableObject.h"
#endif // !MovableObject

/**
 * Implementation of MovableObject class
 *
 * Derivative of GameObject. Contains behavior for movable objects. (Important: Collision checks are only executed going out from MovableObjects, not GameObject)
*/
class PlayerObject : public MovableObject
{
	int coins = 0; // Count of collected coins
	int score = 0; // Current score of the player
public:
	std::shared_ptr<sf::View> playerCamera; // Player Camera (sf::View - Moves along with the player)
	bool isAlive = true; // State defining if the player is alive or not

	std::vector<std::function<void(int)>> OnCoinAmountChange; // Functions can be added which will be called when the coin count changes.
	std::vector<std::function<void(int)>> OnScoreChange; // Functions can be added which will be called when the score count changes.

	PlayerObject(float width, float height);
	PlayerObject();
	~PlayerObject();

	void Start() override;
	void Update() override;
	void Destroy() override;
	void SetPosition(sf::Vector2f pos) override;
	void OnKeyUp(sf::Keyboard::Key key) override;
	void OnKeyDown(sf::Keyboard::Key key) override;
	void OnTriggerEnter(std::shared_ptr<Collision> collider) override;
	virtual void Move(sf::Vector2f movementVector);

	void AddCoins(int amount);
	int GetCoinCount();
	void AddPoints(int amount);
	int GetScore();

	void ThrowTrap();
};

