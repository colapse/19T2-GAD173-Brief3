#pragma once
#ifndef SFML_GRAPHICS_HPP
#include <SFML/Graphics.hpp>
#endif // !_SFML_GRAPHICS_
class AnimatedSprite :
	public sf::Sprite
{
public:
	AnimatedSprite();
	~AnimatedSprite();

	float frameChangeCooldown = 0;
	int fps = 10;
	float tileSize = 0;
	int frameCountX = 1;
	int frameCountY = 1;

	int currentFrameX = 0;
	int currentFrameY = 0;

	void Update();
};

