#pragma once
#include "MovableObject.h"

#ifndef SFML_GRAPHICS_HPP
#include <SFML/Graphics.hpp>
#endif // !_SFML_GRAPHICS_

#ifndef MovableObject
#include "MovableObject.h"
#endif // !MovableObject
class Enemy :
	public MovableObject
{
public:
	Enemy();
	~Enemy();
};

