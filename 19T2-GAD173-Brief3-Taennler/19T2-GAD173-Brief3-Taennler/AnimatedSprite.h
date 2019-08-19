#pragma once
#ifndef SFML_GRAPHICS_HPP
#include <SFML/Graphics.hpp>
#endif // !_SFML_GRAPHICS_

/**
 * Implementation of AnimatedSprite class
 *
 * AnimatedSprite is a derivation of sf::Sprite. It allows you to create an simple animated sprite out of a spritesheet.
 * It only supports spritesheets in which all tiles have the same size.
 * Flaws: If the spritesheet doesn't contain a completely filled in "table", some empty frames will be displayed at some point.
*/
class AnimatedSprite :
	public sf::Sprite
{
	float frameChangeCooldown = 0; // Internal cooldown countdown until the next frame will be displayed.
	int currentFrameX = 0; // The current horizontal frame position
	int currentFrameY = 0; // The current vertical frame position
public:
	AnimatedSprite();
	~AnimatedSprite();

	int fps = 10; // Setting: How many frames per second
	float tileSize = 0; // Setting: Size of each tile within the spritesheet
	int frameCountX = 1; // Setting: The amount of frames horizontally
	int frameCountY = 1; // Setting: The amount of frames vertically


	void Update();
};

