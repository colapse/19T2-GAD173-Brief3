#include "pch.h"
#include "View.h"


View::View()
{
	_viewRect = new sf::RectangleShape();
}


View::View(std::string pViewName, float width, float height) : viewName(pViewName) {
	_viewRect = new sf::RectangleShape(sf::Vector2f(width, height));
}

View::View(std::string pViewName) : viewName(pViewName) {
	_viewRect = new sf::RectangleShape();
}

View::~View()
{
	// Delete all Buttons
	if (_viewButtons.size() > 0)
	{
		for (Button * b : _viewButtons) {
			delete b;
		}
		_viewButtons.clear();
	}
	// Delete all sf::Shapes
	if (_viewShapes.size() > 0)
	{
		for (sf::Shape * s : _viewShapes) {
			delete s;
		}
		_viewShapes.clear();
	}
	// Delete all sf::Texts
	if (_viewTexts.size() > 0)
	{
		for (sf::Text * t : _viewTexts) {
			delete t;
		}
		_viewTexts.clear();
	}

	// Clear Drawable array
	if (_drawables.size() > 0)
	{
		_drawables.clear();
	}

	// Delete view rect & set a nullpointer
	if (_viewRect != nullptr) {
		delete _viewRect;
		_viewRect = nullptr;
	}

	// Delete sf::view & set a nullpointer
	if (sfView != nullptr) {
		delete sfView;
		sfView = nullptr;
	}
}


void View::SetViewName(std::string pViewName) {
	viewName = pViewName;
}

void View::SetViewSize(float width, float height) {

}

void View::AddButton(Button * pButton) {
	AddButton(pButton, false);
}

void View::AddButton(Button * pButton, bool inFront) {
	// Exit if the given button is a nullpointer
	if (pButton == nullptr)
		return;

	// Check if the button should be added in the front of the list
	if (inFront) {
		// Insert button to the _viewButtons
		_viewButtons.insert(_viewButtons.begin(), pButton);

		// Add the buttons shape to the _drawables list if set 
		if (pButton->GetShapeObject() != nullptr)
			_drawables.insert(_drawables.begin(), pButton->GetShapeObject());

		// Add the buttons sprite to the _drawables list if set
		if (pButton->GetSpriteObject() != nullptr)
			_drawables.insert(_drawables.begin(), pButton->GetSpriteObject());

		// Add the buttons text to the _drawables list if set
		if (pButton->GetTextObject() != nullptr)
			_drawables.insert(_drawables.begin(), pButton->GetTextObject());
	}
	else {
		// Insert button to the _viewButtons
		_viewButtons.push_back(pButton);

		// Add the buttons shape to the _drawables list if set 
		if (pButton->GetShapeObject() != nullptr)
			_drawables.push_back(pButton->GetShapeObject());

		// Add the buttons sprite to the _drawables list if set
		if (pButton->GetSpriteObject() != nullptr)
			_drawables.push_back(pButton->GetSpriteObject());

		// Add the buttons text to the _drawables list if set
		if (pButton->GetTextObject() != nullptr)
			_drawables.push_back(pButton->GetTextObject());
	}
}


void View::AddShape(sf::Shape * pShape) {
	AddShape(pShape, false);
}

void View::AddShape(sf::Shape * pShape, bool inFront) {
	// Exit if the given Shape is a nullpointer
	if (pShape == nullptr)
		return;

	if (inFront) {
		// Insert Shape to the _viewShapes list
		_viewShapes.insert(_viewShapes.begin(), pShape);
		// Add the Shape to the _drawables list if set
		if (pShape != nullptr)
			_drawables.insert(_drawables.begin(), pShape);
	}
	else {
		// Insert Shape to the _viewShapes list
		_viewShapes.push_back(pShape);
		// Add the Shape to the _drawables list if set
		if (pShape != nullptr)
			_drawables.push_back(pShape);
	}
}
void View::AddText(sf::Text * pText) {
	AddText(pText, false);
}

void View::AddText(sf::Text * pText, bool inFront) {
	// Exit if the given Text is a nullpointer
	if (pText == nullptr)
		return;

	if (inFront) {
		// Insert Text to the _viewTexts list
		_viewTexts.insert(_viewTexts.begin(), pText);
		// Add the Text to the _drawables list if set
		if (pText != nullptr)
			_drawables.insert(_drawables.begin(), pText);
	}
	else {
		// Insert Text to the _viewTexts list
		_viewTexts.push_back(pText);
		// Add the Text to the _drawables list if set
		if (pText != nullptr)
			_drawables.push_back(pText);
	}
}

void View::AddSprite(sf::Sprite * pSprite) {
	AddSprite(pSprite, false);
}

void View::AddSprite(sf::Sprite * pSprite, bool inFront) {
	// Exit if the given Sprite is a nullpointer
	if (pSprite == nullptr)
		return;

	if (inFront) {
		// Insert Sprite to the _viewSprites list
		_viewSprites.insert(_viewSprites.begin(), pSprite);
		// Add the Sprite to the _drawables list if set
		if (pSprite != nullptr)
			_drawables.insert(_drawables.begin(), pSprite);
	}
	else {
		// Insert Sprite to the _viewSprites list
		_viewSprites.push_back(pSprite);
		// Add the Sprite to the _drawables list if set
		if (pSprite != nullptr)
			_drawables.push_back(pSprite);
	}
}

void View::AddDrawable(sf::Drawable * drawable) {
	AddDrawable(drawable, false);
}

void View::AddDrawable(sf::Drawable * drawable, bool inFront) {
	// Exit if the given Drawable is a nullpointer
	if (drawable == nullptr)
		return;

	if (inFront)
		_drawables.insert(_drawables.begin(), drawable); // Insert Drawable to the _drawables list
	else
		_drawables.push_back(drawable); // Insert Drawable to the _drawables list
}

void View::SetSfView(sf::View * pSfView) {
	sfView = pSfView;
}

std::vector<Button*> View::GetButtons() {
	return _viewButtons;
}

std::vector<TextInput*> View::GetTextInputs() {
	return _viewTextInputs;
}

std::vector<sf::Shape*> View::GetShapes() {
	return _viewShapes;
}

std::vector<sf::Text*> View::GetTexts() {
	return _viewTexts;
}

std::vector<sf::Sprite*> View::GetSprites() {
	return _viewSprites;
}

std::vector<sf::Drawable*> View::GetDrawables() {
	return _drawables;
}

sf::View* View::GetSFView()
{
	return sfView;
}

std::string View::GetViewName() {
	return viewName;
}

sf::RectangleShape * View::GetViewRect() {
	return _viewRect;
}
