#pragma once
#ifndef SFML_GRAPHICS_HPP
#include <SFML/Graphics.hpp>
#endif // !SFML_GRAPHICS_HPP
#ifndef Button
#include "Button.h"
#endif // !Button
#ifndef string
#include <string>
#endif // !string

/** Class View
 * Stores all drawable elements of a view/page (Don't confuse with sf::view).
*/
class View
{
	// Prvate Variables: Components
	sf::View * sfView;
	sf::RectangleShape * _viewRect;

	// Prvate Variables: Component Lists
	std::vector<Button*> _viewButtons;
	std::vector<sf::Shape*> _viewShapes;
	std::vector<sf::Text*> _viewTexts;
	std::vector<sf::Sprite*> _viewSprites;

	// Contains all components which can/will be drawn when the View is active
	std::vector<sf::Drawable*> _drawables;

	// Prvate Variables: Component Lists
	std::string viewName;
public:
	// Default Constructor
	View();

	/** View Constructor
		 * @param pViewName New name of the view
		*/
	View(std::string pViewName);

	/** View Constructor
		 * @param pViewName New name of the View
		 * @param width Width of the View
		 * @param height Height of the View
		*/
	View(std::string pViewName, float width, float height);

	// Destructor
	~View();

	/** SetViewName: Give the view a name
		 * @param pViewName New name of the view
		*/
	void SetViewName(std::string viewName);

	/** SetViewSize: Sets the size of the view
		 * @param width New width of the view
		 * @param height New height of the view
		*/
	void SetViewSize(float width, float height);

	/** AddButton: Add a button to the view
		 * @param pButton* Pointer to the button
		*/
	void AddButton(Button * button);

	/** AddButton: Add a button to the view
		 * @param pButton* Pointer to the button
		 * @param inFront Insert the button at the front or end of the list
		*/
	void AddButton(Button * button, bool inFront);

	/** AddShape: Add a Shape to the view
		 * @param pShape* Pointer to the Shape
		*/
	void AddShape(sf::Shape * textInput);

	/** AddShape: Add a Shape to the view
		 * @param pShape* Pointer to the Shape
		 * @param inFront Insert the Shape at the front or end of the list
		*/
	void AddShape(sf::Shape * textInput, bool inFront);

	/** AddText: Add a Text to the view
		 * @param pText* Pointer to the Text
		*/
	void AddText(sf::Text * textInput);

	/** AddText: Add a Text to the view
		 * @param pText* Pointer to the Text
		 * @param inFront Insert the Text at the front or end of the list
		*/
	void AddText(sf::Text * textInput, bool inFront);

	/** AddSprite: Add a Sprite to the view
		 * @param pSprite* Pointer to the Sprite
		*/
	void AddSprite(sf::Sprite * sprite);

	/** AddSprite: Add a Sprite to the view
		 * @param pSprite* Pointer to the Sprite
		 * @param inFront Insert the Sprite at the front or end of the list
		*/
	void AddSprite(sf::Sprite * sprite, bool inFront);

	/** AddDrawable: Add a Drawable to the view
		 * @param drawable* Pointer to a child class of Drawable
		*/
	void AddDrawable(sf::Drawable * drawable);

	/** AddDrawable: Add a Drawable to the view
		 * @param drawable* Pointer to a child class of Drawable
		 * @param inFront Insert the Drawable at the front or end of the list
		*/
	void AddDrawable(sf::Drawable * textInput, bool inFront);

	/** SetSfView: Set sf::View of the View
		 * @param pSfView* Pointer to the sf::View
		*/
	void SetSfView(sf::View * pSfView);

	/** GetButtons: Returns a Vector with pointers to the buttons of the view
		 * @return std::vector<Button*>
		*/
	std::vector<Button*> GetButtons();


	/** GetShapes: Returns a Vector with pointers to the Shapes of the view
		 * @return std::vector<Shape*>
		*/
	std::vector<sf::Shape*> GetShapes();

	/** GetTexts: Returns a Vector with pointers to the Texts of the view
		 * @return std::vector<Sprite*>
		*/
	std::vector<sf::Text*> GetTexts();

	/** GetSprites: Returns a Vector with pointers to the Sprites of the view
		 * @return std::vector<Sprite*>
		*/
	std::vector<sf::Sprite*> GetSprites();

	/** GetDrawables: Returns a Vector with pointers to the Drawables of the view
		 * @return std::vector<Drawable*>
		*/
	std::vector<sf::Drawable*> GetDrawables();

	/** GetSFView: Returns a pointer to the sf::View
		 * @return sf::View*
		*/
	sf::View* GetSFView();

	/** GetViewName: Returns the name of the view
		 * @return string
		*/
	std::string GetViewName();

	/** GetViewRect: Returns the sf::RectangleShape of the view
		 * @return sf::RectangleShape *
		*/
	sf::RectangleShape * GetViewRect();
};

