#include "pch.h"
#include "GameObject.h"


GameObject::GameObject()
{
	collider = sf::RectangleShape(sf::Vector2f(32, 32)); // TODO make dynamic tile size
}

GameObject::GameObject(float width, float height)
{
	collider = sf::RectangleShape(sf::Vector2f(width, height)); 
}


GameObject::~GameObject()
{
	Destroy();
}

void GameObject::SetPosition(sf::Vector2f pos) {
	this->pos = pos;
	collider.setPosition(pos);
	sprite.setPosition(pos);
}

sf::Vector2f GameObject::GetPosition() {
	return this->pos;
}

void GameObject::Start() {
	if(isSolid)
		collider.setFillColor(sf::Color::Red); // For Debug
}

void GameObject::Update() {
	
}

void GameObject::OnKeyUp(sf::Keyboard::Key key) {
	// TODO
}

void GameObject::OnKeyDown(sf::Keyboard::Key key) {
	// TODO
}

void GameObject::OnCollisionEnter(std::shared_ptr<Collision> collider) {
	// TODO
}

void GameObject::OnTriggerEnter(std::shared_ptr<Collision> collider) {
	// TODO
}

void GameObject::Destroy() {
	for (std::function<void()> func : OnRequestDestroy) {
		func();
	}
}
