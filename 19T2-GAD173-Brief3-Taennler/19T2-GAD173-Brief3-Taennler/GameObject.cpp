#include "pch.h"
#include "GameObject.h"


GameObject::GameObject()
{
	sprite = new sf::Sprite;
	collider = sf::RectangleShape(sf::Vector2f(32, 32)); // Create collider of gameobject
}

GameObject::GameObject(float width, float height)
{
	sprite = new sf::Sprite;
	collider = sf::RectangleShape(sf::Vector2f(width, height)); // Create collider of the gameobject
}


GameObject::~GameObject()
{
	Destroy();
	prefab = nullptr;
	if (sprite != nullptr) {
		delete sprite;
		sprite = nullptr;
	}
}

/** Method SetPosition: Updates the position of the gameobject. */
void GameObject::SetPosition(sf::Vector2f pos) {
	this->pos = pos;
	collider.setPosition(pos);
	sprite->setPosition(pos);
}

/** Method GetPosition: Returns the current position of the gameobject. */
sf::Vector2f GameObject::GetPosition() {
	return this->pos;
}

/** Method Start: This function is called once after the object gets created. */
void GameObject::Start() {
	if(isSolid)
		collider.setFillColor(sf::Color::Red); // For Debug
}

/** Method Update: This function is called once every frame. */
void GameObject::Update() {
	
}

void GameObject::OnKeyUp(sf::Keyboard::Key key) {}

void GameObject::OnKeyDown(sf::Keyboard::Key key) {}

void GameObject::OnCollisionEnter(std::shared_ptr<Collision> collider) {}

void GameObject::OnTriggerEnter(std::shared_ptr<Collision> collider) {}

/** Method Destroy: Unloads some stuff from the object and informs listeners that it wants to be destroyed. */
void GameObject::Destroy() {
	
	// Call listener functions
	for (std::function<void()> func : OnRequestDestroy) {
		func();
	}
}
