#include "pch.h"
#include "Button.h"


Button::Button()
{
	_txtText = std::make_shared<sf::Text>();

	isMouseOver = false;
	isMousePressed = false;
	isButtonDown = false;
	minTextMargin = 3;
	isActive = true;
}


Button::~Button()
{
	if (_txtText != nullptr) {
		_txtText = nullptr;
	}
	if (_shape != nullptr) {
		_shape = nullptr;
	}
	if (_sprite != nullptr) {
		_sprite = nullptr;
	}
}

/** OnButtonDown: Executes functions when button is pressed down. (Happens only once until button is released again) */
void Button::OnButtonDown() {
	// If the button is currently held down, exit
	if (isButtonDown)
		return;

	// Loop through btnDownFuncs list
	for (std::function<void()> func : btnDownFuncs) {
		func();
	}
}

/** SetAutoCharacterSize: Finds the perfect character size for the text component to fit within the buttons bounds */
void Button::SetAutoCharacterSize() {
	// Exit if either the _shape or _txtText component is null. (Avoid access violations)
	if (_shape == nullptr || _txtText == nullptr)
		return;

	// Calculate the horizontal ratio between the buttons & texts(+ minium margin) width. (Basically checking if the text is overflowing the button horizontally or if its too small.)
	float horRatio = _shape->getLocalBounds().width / (_txtText->getGlobalBounds().width + minTextMargin * 2);
	// Calculate a new character size depending on the horizontal ratio
	int newSize = _txtText->getCharacterSize() * horRatio;

	// Calculate the vertical ratio between the buttons & texts(+ minium margin) height. (Basically checking if the text is overflowing the button vertically or if its too small.)
	float vertRatio = _shape->getLocalBounds().height / (2 * newSize + minTextMargin * 2);
	// Check if the text is overflowing vertically with the new character size
	if (vertRatio < 1) {
		// Modify the new character size to fit in vertically
		newSize *= vertRatio;
	}

	// Set the new character size
	_txtText->setCharacterSize(newSize);
}

/** SetTextPosition: Centers the text within the button. */
void Button::SetTextPosition() {
	if (_shape != nullptr || _txtText != nullptr) { // Avoid access violations
		// Calculate & set the center position depending on the texts & buttons boundaries
		_txtText->setPosition((_shape->getGlobalBounds().left + _shape->getGlobalBounds().width / 2 - (_txtText->getGlobalBounds().width / 2)), (_shape->getGlobalBounds().top + _shape->getGlobalBounds().height / 2 - (_txtText->getGlobalBounds().height / 2)));
	}
}

#pragma region Button Event Handling
/** SetButtonPressed: Sets the buttons press state to true & executes button pressed functions. (Continously until button isn't pressed down anymore) */
void Button::SetButtonPressed() {
	// Check if the button is already set as pressed down
	if (!isButtonDown) {
		OnButtonDown(); // OnButtonDown() will execute one-time functions when the button is pressed down
		isButtonDown = true; // Set the buttons state to pressed down
	}

	// Iterate through all btnPressedFuncs (Functions that will be executed over'n'over again while the button is pressed down.)
	for (std::function<void()> func : btnPressedFuncs) {
		func();
	}
}

/** SetButtonReleased: Sets the buttons press state to false & executes button released functions (Once). */
void Button::SetButtonReleased() {
	// Exit if the buttons pressed state is already set to false
	if (!isButtonDown)
		return;

	isButtonDown = false; // Set the buttons state to pressed down to false

	// Iterate through all btnReleasedFuncs (Functions that will be executed once when the button is released.)
	for (std::function<void()> func : btnReleasedFuncs) {
		func();
	}
}

/** SetMouseEnter: Sets the mouse over state to true & executes mouse enter functions (Once). */
void Button::SetMouseEnter() {
	// Exit if the mouse over state is already set to true
	if (isMouseOver)
		return;

	isMouseOver = true; // Set mouse over state to true

	// Iterate through all mouseEnterFuncs (Functions that will be executed once when the mouse enters the buttons boundaries.)
	for (std::function<void()> func : mouseEnterFuncs) {
		func();
	}
}

/** SetMouseExit: Sets the mouse over state to false & executes mouse exit functions (Once). */
void Button::SetMouseExit() {
	// Exit if the mouse over state is already set to false
	if (!isMouseOver)
		return;

	isMouseOver = false; // Set mouse over state to false

	// Iterate through all mouseExitFuncs (Functions that will be executed once when the mouse exits the buttons boundaries.)
	for (std::function<void()> func : mouseExitFuncs) {
		func();
	}
}

/** AddButtonDownFunc: Add a new function which will be called once when the button is pressed down.
 * @param func The function (F.ex. a lambda function) to be added
*/
void Button::AddButtonDownFunc(std::function<void()> func) {
	btnDownFuncs.push_back(func);
}

/** AddButtonPressedFunc: Add a new function which will be called over'n'over again while the button is pressed down.
 * @param func The function (F.ex. a lambda function) to be added
*/
void Button::AddButtonPressedFunc(std::function<void()> func) {
	btnPressedFuncs.push_back(func);
}

/** AddButtonReleasedFunc: Add a new function which will be called once when the button is released.
 * @param func The function (F.ex. a lambda function) to be added
*/
void Button::AddButtonReleasedFunc(std::function<void()> func) {
	btnReleasedFuncs.push_back(func);
}

/** AddMouseEnterFunc: Add a new function which will be called once when the mouse enters the buttons boundaries.
 * @param func The function (F.ex. a lambda function) to be added
*/
void Button::AddMouseEnterFunc(std::function<void()> func) {
	mouseEnterFuncs.push_back(func);
}

/** AddButtonDownFunc: Add a new function which will be called once when the mouse exits the buttons boundaries.
 * @param func The function (F.ex. a lambda function) to be added
*/
void Button::AddMouseExitFunc(std::function<void()> func) {
	mouseExitFuncs.push_back(func);
}

/* === Following functions will enable to remove button event functions - NOT FULLY IMPLEMENTED YET.
// Remove Functions
void Button::RemoveButtonDownFunc(std::function<void()> * func) {
	for (int i = 0; i < btnDownFuncs.size; i++) {
		if (&btnDownFuncs[i] == func) {
			btnDownFuncs.erase(btnDownFuncs.begin()+i);
			break;
		}
	}
}

void Button::RemoveButtonPressedFunc(std::function<void()> * func) {
	for (int i = 0; i < btnPressedFuncs.size; i++) {
		if (&btnPressedFuncs[i] == func) {
			btnPressedFuncs.erase(btnPressedFuncs.begin() + i);
			break;
		}
	}
}

void Button::RemoveButtonReleasedFunc(std::function<void()> * func) {
	for (int i = 0; i < btnReleasedFuncs.size; i++) {
		if (&btnReleasedFuncs[i] == func) {
			btnReleasedFuncs.erase(btnReleasedFuncs.begin() + i);
			break;
		}
	}
}

void Button::RemoveMouseEnterFunc(std::function<void()> * func) {
	for (int i = 0; i < mouseEnterFuncs.size; i++) {
		if (&mouseEnterFuncs[i] == func) {
			mouseEnterFuncs.erase(mouseEnterFuncs.begin() + i);
			break;
		}
	}
}

void Button::RemoveMouseExitFunc(std::function<void()> * func) {
	for (int i = 0; i < mouseExitFuncs.size; i++) {
		if (&mouseExitFuncs[i] == func) {
			mouseExitFuncs.erase(mouseExitFuncs.begin() + i);
			break;
		}
	}
}*/
#pragma endregion

/** SetShape: Assign a pointer to a shape (This can be a RectangularShape, CircleShape or any other child class of sf::Shape
	 * @param shape The pointer to the shape
	*/
void Button::SetShape(std::shared_ptr<sf::Shape> shape) {
	this->_shape = nullptr;

	this->_shape = shape; // Assign the shape pointer
	SetTextPosition(); // This function call will realign the text to the new boundaries of the button
}

/** SetShape: Assign a pointer to a shape (This can be a RectangularShape, CircleShape or any other child class of sf::Shape
	 * @param shape The pointer to the shape
	*/
void Button::SetSprite(std::shared_ptr < sf::Sprite> sprite) {
	this->_sprite = nullptr;

	this->_sprite = sprite; // Assign the shape pointer
}

/** SetPosition: Sets the position of the button
 * @param x The new x position
 * @param y The new y position
*/
void Button::SetPosition(float x, float y) {
	// Exit if the shape isn't set (Avoid access violations)
	if (_shape == nullptr)
		return;
	_shape->setPosition(x, y); // Set the shapes global position
	SetTextPosition(); // This function call will moves the text component along with the new shape position
}

/** ScaleSize: Scale the buttons shape
 * @param scaleX Horizontal scale factor
 * @param scaleY Vertical scale factor
*/
void Button::ScaleSize(float scaleX, float scaleY) {
	// Exit if the shape isn't set (Avoid access violations)
	if (_shape == nullptr)
		return;
	_shape->setScale(scaleX, scaleY); // Sets the shapes new scale
	SetAutoCharacterSize(); // Resize the text to fit within the new shapes size
}

/** SetFillColor: Sets the fill/background color of the buttons shape
 * @param color The color for the new background
*/
void Button::SetFillColor(sf::Color color) {
	// Exit if the shape isn't set (Avoid access violations)
	if (_shape == nullptr)
		return;
	_shape->setFillColor(color); // Assigns the given color to the shape
}

/** SetOutline: Set up an outline for the buttons shape
 * @param thickness The desired thickness of the buttons outline
 * @param color The desired color of the buttons background
*/
void Button::SetOutline(float thickness, sf::Color color) {
	// Exit if the shape isn't set (Avoid access violations)
	if (_shape == nullptr)
		return;
	_shape->setOutlineColor(color); // Assign the outline color to the shape
	_shape->setOutlineThickness(thickness); // Assign the outline thickness to the shape
}

/** SetFont: Set the font of the text component
 * @param font The desired font to be used for the text component
*/
void Button::SetFont(sf::Font font) {
	// Exit if the text isn't set (Avoid access violations)
	if (_txtText == nullptr)
		return;

	this->font = sf::Font::Font(font); // Store the font in the class instance
	_txtText->setFont(this->font); // Assign the new font to the text component
	SetAutoCharacterSize(); // Automatically resize the character size to fit within the buttons boundaries with the new font
	SetTextPosition(); // Reset the position of the text component to the center of the buttons boundaries
}

/** SetText: Set a text to be displayed in the button & its color
 * @param text Desired string to be displayed in the button
 * @param color Desired color of the text
*/
void Button::SetText(std::string text, sf::Color color) {
	// Check if the text component is a null pointer
	if (_txtText == nullptr) {
		_txtText = std::make_shared<sf::Text>(); // Create a new sf::Text instance and assign it
		_txtText->setFont(this->font); // Set the font of the new text component
	}


	_txtText->setString(text); // Sets the given string in the text component
	_txtText->setFillColor(color); // Sets the given color of the text
	SetAutoCharacterSize(); // Automatically resize the character size to fit within the buttons boundaries
	SetTextPosition(); // Reset the position of the text component to the center of the buttons boundaries
}

/** SetText: Set a text to be displayed in the button (With default color black)
 * @param text Desired string to be displayed in the button
*/
void Button::SetText(std::string text) {
	SetText(text, sf::Color::Black); // Sets the text of the text component and set default color black
}

/** GetTextObject: Returns the pointer to the text component
 * @return sf::Text The pointer to the Text component
*/
std::shared_ptr < sf::Text > Button::GetTextObject() {
	return _txtText;
}

/** GetShapeObject: Returns the pointer to the shape component
 * @return sf::Shape The pointer to the shape
*/
std::shared_ptr < sf::Shape > Button::GetShapeObject() {
	return _shape;
}

/** GetSpriteObject: Returns the pointer to the shape component
 * @return sf::Shape The pointer to the shape
*/
std::shared_ptr < sf::Sprite > Button::GetSpriteObject() {
	return _sprite;
}

/** GetGlobalBounds: Returns the global bounds of the shape component
 * @return sf::FloatRect the bounds of the shape
*/
sf::FloatRect Button::GetGlobalBounds() {
	// Exit if the shape isn't set (Avoid access violations)
	if (_shape == nullptr)
		return sf::FloatRect();
	return _shape->getGlobalBounds();
}