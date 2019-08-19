#include "pch.h"
#include "AnimatedSprite.h"

#ifndef _IOSTREAM_
#include <iostream>
#endif // !_IOSTREAM_

#ifndef Level
#include "Level.h"
#endif // !Level

AnimatedSprite::AnimatedSprite()
{
}


AnimatedSprite::~AnimatedSprite()
{
}

/** Update: Swaps to the next tile(/frame) of the animated sprite and sets cooldowns. */
void AnimatedSprite::Update() {
	if (getTexture() == nullptr || Level::instance == nullptr)
		return;
	
	if (frameChangeCooldown <= 0) {

		if (currentFrameX + 1 >= frameCountX)
			currentFrameY = currentFrameY + 1 >= frameCountY ? 0 : currentFrameY + 1;
		currentFrameX = currentFrameX+1 >= frameCountX ? 0 : currentFrameX + 1;

		sf::IntRect newRect((int)(currentFrameX*tileSize), (int)(currentFrameY*tileSize), (int)tileSize, (int)tileSize);

		setTextureRect(newRect);


		frameChangeCooldown = 1/fps;
	}
	else {
		frameChangeCooldown -= Level::deltaTime;
	}
}
