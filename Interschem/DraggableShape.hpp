#ifndef DRAGGABLE_SHAPE_HPP
#define DRAGGABLE_SHAPE_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class DraggableShape {
public:
    DraggableShape(const sf::Font& font, const std::vector<sf::Vector2f>& points, sf::Color color, const sf::Vector2f& position, bool editable = true);

    void updateText();
    void startDrag(const sf::Vector2i& mousePos);
    void drag(const sf::Vector2i& mousePos);
    void stopDrag();
    void handleTextInput(const sf::Event& event);
    bool isMouseOver(const sf::Vector2i& mousePos) const;
    int getAnchorUnderMouse(const sf::Vector2i& mousePos) const;
    sf::Vector2f getAnchorPosition(int anchorIndex) const;
    void draw(sf::RenderWindow& window) const;

    bool isSelected;
    bool isEditable;

    sf::ConvexShape shape;
    sf::Text text;
    std::string userInput;

private:
    void updateTextPosition();

    bool isBeingDragged;
    sf::Vector2f dragOffset;
    std::vector<sf::CircleShape> anchors;
};

#endif