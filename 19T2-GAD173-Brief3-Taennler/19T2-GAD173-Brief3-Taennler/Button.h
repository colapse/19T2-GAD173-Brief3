#pragma once

#ifndef SFML_GRAPHICS_HPP
#include <SFML/Graphics.hpp>
#endif // !SFML_GRAPHICS_HPP
#ifndef functional
#include <functional>
#endif // !functional
/**
 * Implementation of Button class
 *
 * Enables easy creation of a button with text and input events (mouse & click). (Sprite in button not supported yet)
*/
class Button
{
	// Private Variables: Input States
	bool isMouseOver;
	bool isMousePressed;
	bool isButtonDown;

	// Private Variables: Button Components
	sf::Font font; // Font used for the text component
	std::shared_ptr <sf::Text> _txtText; // Text component of the button
	std::shared_ptr<sf::Shape> _shape; // Pointer to a shape. (=> Buttons "boundaries")
	std::shared_ptr <sf::Sprite> _sprite; // (Optional) Used for an image button

	// Private Variables: Input Event Functions. They store functions (Such as lambda functions) which will be executed on specific input events
	std::vector<std::function<void()>> btnDownFuncs;
	std::vector<std::function<void()>> btnPressedFuncs;
	std::vector<std::function<void()>> btnReleasedFuncs;
	std::vector<std::function<void()>> mouseEnterFuncs;
	std::vector<std::function<void()>> mouseExitFuncs;

	// Private Variables: Settings
	float minTextMargin; // The minimum space between the texts boundaries and the buttons border

	/** OnButtonDown: Executes functions when button is pressed down. (Happens only once until button is released again) */
	void OnButtonDown();

	/** SetAutoCharacterSize: Finds the perfect character size for the text component to fit within the buttons bounds */
	void SetAutoCharacterSize();

	/** SetTextPosition: Centers the text within the button. */
	void SetTextPosition();

public:
	// Public Variables: States
	bool isActive; // Defines if the button is activated or not

#pragma region Button Event Handling
	/** SetButtonPressed: Sets the buttons press state to true & executes button pressed functions. (Continously until button isn't pressed down anymore) */
	void SetButtonPressed();

	/** SetButtonReleased: Sets the buttons press state to false & executes button released functions (Once). */
	void SetButtonReleased();

	/** SetMouseEnter: Sets the mouse over state to true & executes mouse enter functions (Once). */
	void SetMouseEnter();

	/** SetMouseExit: Sets the mouse over state to false & executes mouse exit functions (Once). */
	void SetMouseExit();

	/** AddButtonDownFunc: Add a new function which will be called once when the button is pressed down.
	 * @param func The function (F.ex. a lambda function) to be added
	*/
	void AddButtonDownFunc(std::function<void()> func);

	/** AddButtonPressedFunc: Add a new function which will be called over'n'over again while the button is pressed down.
	 * @param func The function (F.ex. a lambda function) to be added
	*/
	void AddButtonPressedFunc(std::function<void()> func);

	/** AddButtonReleasedFunc: Add a new function which will be called once when the button is released.
	 * @param func The function (F.ex. a lambda function) to be added
	*/
	void AddButtonReleasedFunc(std::function<void()> func);

	/** AddMouseEnterFunc: Add a new function which will be called once when the mouse enters the buttons boundaries.
	 * @param func The function (F.ex. a lambda function) to be added
	*/
	void AddMouseEnterFunc(std::function<void()> func);

	/** AddButtonDownFunc: Add a new function which will be called once when the mouse exits the buttons boundaries.
	 * @param func The function (F.ex. a lambda function) to be added
	*/
	void AddMouseExitFunc(std::function<void()> func);

	/* === Following functions will enable to remove button event functions - NOT FULLY IMPLEMENTED YET.
	// Remove Functions
	void RemoveButtonDownFunc(std::function<void()> * func);

	void RemoveButtonPressedFunc(std::function<void()> * func);

	void RemoveButtonReleasedFunc(std::function<void()> * func);

	void RemoveMouseEnterFunc(std::function<void()> * func);

	void RemoveMouseExitFunc(std::function<void()> * func);*/
#pragma endregion

	/** Default Constructor for Button class */
	Button();
	/** Deconstructor for Button class */
	~Button();

	/** SetShape: Assign a pointer to a shape (This can be a RectangularShape, CircleShape or any other child class of sf::Shape
	 * @param shape The pointer to the shape
	*/
	void SetShape(std::shared_ptr<sf::Shape> shape);

	void SetSprite(std::shared_ptr<sf::Sprite> shape);

	/** SetPosition: Sets the position of the button
	 * @param x The new x position
	 * @param y The new y position
	*/
	void SetPosition(float x, float y);

	/** ScaleSize: Scale the buttons shape
	 * @param scaleX Horizontal scale factor
	 * @param scaleY Vertical scale factor
	*/
	void ScaleSize(float scaleX, float scaleY);

	/** SetFillColor: Sets the fill/background color of the buttons shape
	 * @param color The color for the new background
	*/
	void SetFillColor(sf::Color color);

	/** SetOutline: Set up an outline for the buttons shape
	 * @param thickness The desired thickness of the buttons outline
	 * @param color The desired color of the buttons background
	*/
	void SetOutline(float thickness, sf::Color color);

	/** SetFont: Set the font of the text component
	 * @param font The desired font to be used for the text component
	*/
	void SetFont(sf::Font font);

	/** SetText: Set a text to be displayed in the button & its color
	 * @param text Desired string to be displayed in the button
	 * @param color Desired color of the text
	*/
	void SetText(std::string text, sf::Color color);

	/** SetText: Set a text to be displayed in the button (With default color black)
	 * @param text Desired string to be displayed in the button
	*/
	void SetText(std::string text);

	/** GetTextObject: Returns the pointer to the text component
	 * @return sf::Text The pointer to the Text component
	*/
	std::shared_ptr < sf::Text > GetTextObject();

	/** GetShapeObject: Returns the pointer to the shape component
	 * @return sf::Shape The pointer to the shape
	*/
	std::shared_ptr < sf::Shape > GetShapeObject();

	/** GetSpriteObject: Returns the pointer to the sprite component
	 * @return sf::Sprite The pointer to the sprite
	*/
	std::shared_ptr < sf::Sprite > GetSpriteObject();

	/** GetGlobalBounds: Returns the global bounds of the shape component
	 * @return sf::FloatRect the bounds of the shape
	*/
	sf::FloatRect GetGlobalBounds();
};

