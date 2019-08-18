#pragma once

#ifndef SFML_GRAPHICS_HPP
#include <SFML/Graphics.hpp>
#endif // !_SFML_GRAPHICS_

#ifndef MovableObject
#include "MovableObject.h"
#endif // !MovableObject

class PlayerObject : public MovableObject
{
	int coins = 0;
	int score = 0;
public:
	std::shared_ptr<sf::View> playerCamera;
	bool isAlive = true;

	std::vector<std::function<void(int)>> OnCoinAmountChange;
	std::vector<std::function<void(int)>> OnScoreChange;

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

