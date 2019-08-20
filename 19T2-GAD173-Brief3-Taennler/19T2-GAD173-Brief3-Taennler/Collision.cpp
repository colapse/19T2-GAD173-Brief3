#include "pch.h"
#include "Collision.h"


Collision::Collision()
{
}


Collision::~Collision()
{
	if (colliderObject != nullptr) {
		colliderObject.reset();
	}
}
