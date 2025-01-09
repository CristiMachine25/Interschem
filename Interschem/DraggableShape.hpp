#ifndef DRAGGABLE_SHAPE_HPP
#define DRAGGABLE_SHAPE_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

enum class ExecutionState {
    NotVisited,
    Visited
};

struct DraggableShape {
    DraggableShape(const sf::Font& font, const std::vector<sf::Vector2f>& points, sf::Color color, const sf::Vector2f& position, bool editable = true);

    ExecutionState state = ExecutionState::NotVisited;
    std::string variableName;
    DraggableShape* nextShape = nullptr;
    DraggableShape* trueShape = nullptr;  // For if blocks
    DraggableShape* falseShape = nullptr; // For if blocks

    void updateText();
    void startDrag(const sf::Vector2i& mousePos);
    void drag(const sf::Vector2i& mousePos);
    void stopDrag();
    sf::Color getFillColor() const {
        return shape.getFillColor();
    }
    void setVariableName(const std::string& varName);
    void setState(ExecutionState newState);
    void setNextShape(DraggableShape* shape);
    void setTrueShape(DraggableShape* shape);
    void setFalseShape(DraggableShape* shape);
    void handleTextInput(const sf::Event& event);
    bool isMouseOver(const sf::Vector2i& mousePos) const;
    int getAnchorUnderMouse(const sf::Vector2i& mousePos) const;
    sf::Vector2f getAnchorPosition(int anchorIndex) const;
    void draw(sf::RenderWindow& window) const;
    void updateTextPosition();

    // New methods for "if" blocks
    bool isIfBlock() const;
    int getTrueAnchor() const;
    int getFalseAnchor() const;

    bool isSelected;
    bool isEditable;
    bool isBeingDragged;
    sf::Vector2f dragOffset;

    sf::ConvexShape shape;
    sf::Text text;
    std::string userInput;
    std::vector<sf::CircleShape> anchors;
};

#endif // DRAGGABLE_SHAPE_HPP