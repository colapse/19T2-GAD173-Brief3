#pragma once
#ifndef SFML_GRAPHICS_HPP
#include <SFML/Graphics.hpp>
#endif // !_SFML_GRAPHICS_

#ifndef MovableObject
#include "MovableObject.h"
#endif // !MovableObject
class EnemyObject :
	public MovableObject
{
	float aiStuckCounter = 0;
	int lastGridPosX = 0;
public:
	bool isAlive = true;

	EnemyObject(float width, float height);
	EnemyObject();
	~EnemyObject();


	void Start() override;
	void Update() override;
	void OnTriggerEnter(std::shared_ptr<Collision> collider) override;

	void AIInput();
};

