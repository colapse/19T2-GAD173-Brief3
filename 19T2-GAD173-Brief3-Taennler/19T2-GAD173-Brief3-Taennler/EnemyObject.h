#pragma once
#ifndef SFML_GRAPHICS_HPP
#include <SFML/Graphics.hpp>
#endif // !_SFML_GRAPHICS_

#ifndef MovableObject
#include "MovableObject.h"
#endif // !MovableObject

/**
 * Implementation of EnemyObject class
 *
 * Derivative of MovableObject. Contains behavior for the enemy AI and some important stats.
*/
class EnemyObject :
	public MovableObject
{
	float aiStuckCounter = 0; // Counter used for counting how long an enemy has been stuck at the same location.
	int lastGridPosX = 0; // Stores the horizontal grid position of the object from last frame
public:
	bool isAlive = true; // State if the enemy is alive

	EnemyObject(float width, float height);
	EnemyObject();
	~EnemyObject();


	void Start() override;
	void Update() override;
	void OnTriggerEnter(std::shared_ptr<Collision> collider) override;

	void AIInput();
	void Die();
};

