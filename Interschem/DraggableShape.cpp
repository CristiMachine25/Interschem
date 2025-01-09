#include "DraggableShape.hpp"
#include <cmath>

DraggableShape::DraggableShape(const sf::Font& font, const std::vector<sf::Vector2f>& points, sf::Color color, const sf::Vector2f& position, bool editable)
    : userInput(""), isBeingDragged(false), isEditable(editable), isSelected(false) {
    shape.setPointCount(points.size());
    for (size_t i = 0; i < points.size(); ++i) {
        shape.setPoint(i, points[i]);
    }
    shape.setFillColor(color);
    shape.setOutlineThickness(2.0f);
    shape.setOutlineColor(sf::Color::Black);
    shape.setPosition(position);

    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Black);
    updateTextPosition();
}

void DraggableShape::updateText() {
    text.setString(userInput);
    updateTextPosition();
}

void DraggableShape::setVariableName(const std::string& varName) {
    variableName = varName;
}

void DraggableShape::setState(ExecutionState newState) {
    state = newState;
}

void DraggableShape::setNextShape(DraggableShape* shape) {
    nextShape = shape;
}

void DraggableShape::setTrueShape(DraggableShape* shape) {
    trueShape = shape;
}

void DraggableShape::setFalseShape(DraggableShape* shape) {
    falseShape = shape;
}

void DraggableShape::updateTextPosition() {
    sf::FloatRect textBounds = text.getLocalBounds();
    sf::FloatRect shapeBounds = shape.getGlobalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2, textBounds.top + textBounds.height / 2);
    text.setPosition(shapeBounds.left + shapeBounds.width / 2, shapeBounds.top + shapeBounds.height / 2);

    float anchorRadius = 12.f;
    sf::Color c = shape.getFillColor();
    anchors.clear();

    if (c == sf::Color::Cyan) {
        // IF shape anchors
        anchors.resize(3, sf::CircleShape(anchorRadius));
        for (auto& a : anchors) {
            a.setFillColor(sf::Color::Black);
            a.setOutlineThickness(2.0f);
            a.setOutlineColor(sf::Color::Black);
        }

        sf::Vector2f shapePos = shape.getPosition();
        sf::Vector2f topAnchorLocal(55.f, 0.f);       // Moved to center-top (halfway between left and right)
        sf::Vector2f rightAnchorLocal(165.f, 55.f);   // Right anchor stays the same
        sf::Vector2f leftAnchorLocal(-55.f, 55.f);    // Left anchor stays the same

        sf::Vector2f globalTop = shapePos + topAnchorLocal;
        sf::Vector2f globalRight = shapePos + rightAnchorLocal;
        sf::Vector2f globalLeft = shapePos + leftAnchorLocal;

        anchors[0].setPosition(globalTop.x - anchorRadius, globalTop.y - anchorRadius);
        anchors[1].setPosition(globalRight.x - anchorRadius, globalRight.y - anchorRadius);
        anchors[2].setPosition(globalLeft.x - anchorRadius, globalLeft.y - anchorRadius);
    }
    else if (c == sf::Color::Yellow || c == sf::Color::Blue) {
        // Afisare and Int shapes
        anchors.resize(2, sf::CircleShape(anchorRadius));
        for (auto& a : anchors) {
            a.setFillColor(sf::Color::Black);
            a.setOutlineThickness(2.0f);
            a.setOutlineColor(sf::Color::Black);
        }

        sf::Vector2f shapePos = shape.getPosition();
        // Center the top anchor
        sf::Vector2f globalTop = shapePos + sf::Vector2f(110.f, -20.f);
        sf::Vector2f globalBottom = shapePos + sf::Vector2f(110.f, 130.f);

        anchors[0].setPosition(globalTop.x - anchorRadius, globalTop.y - anchorRadius);
        anchors[1].setPosition(globalBottom.x - anchorRadius, globalBottom.y - anchorRadius);
    }
    else if (c == sf::Color::Green) {
        // Start shape
        anchors.resize(1, sf::CircleShape(anchorRadius));
        anchors[0].setFillColor(sf::Color::Black);
        anchors[0].setOutlineThickness(2.0f);
        anchors[0].setOutlineColor(sf::Color::Black);

        sf::Vector2f shapePos = shape.getPosition();
        // Center the anchor
        sf::Vector2f startAnchorLocal(110.f, 70.f);
        sf::Vector2f globalPos = shapePos + startAnchorLocal;
        anchors[0].setPosition(globalPos.x - anchorRadius, globalPos.y - anchorRadius);
    }
    else if (c == sf::Color::Red) {
        // Stop shape
        anchors.resize(1, sf::CircleShape(anchorRadius));
        anchors[0].setFillColor(sf::Color::Black);
        anchors[0].setOutlineThickness(2.0f);
        anchors[0].setOutlineColor(sf::Color::Black);

        sf::Vector2f shapePos = shape.getPosition();
        // Center the anchor
        sf::Vector2f stopAnchorLocal(110.f, -20.f);
        sf::Vector2f globalPos = shapePos + stopAnchorLocal;
        anchors[0].setPosition(globalPos.x - anchorRadius, globalPos.y - anchorRadius);
    }
}

void DraggableShape::startDrag(const sf::Vector2i& mousePos) {
    isBeingDragged = true;
    dragOffset = shape.getPosition() - static_cast<sf::Vector2f>(mousePos);
}

void DraggableShape::drag(const sf::Vector2i& mousePos) {
    if (isBeingDragged) {
        shape.setPosition(static_cast<sf::Vector2f>(mousePos) + dragOffset);
        updateTextPosition();
    }
}

void DraggableShape::stopDrag() {
    isBeingDragged = false;
}

void DraggableShape::handleTextInput(const sf::Event& event) {
    if (isEditable && isSelected && event.type == sf::Event::TextEntered) {
        if (event.text.unicode == 8 && !userInput.empty()) { // backspace
            userInput.pop_back();
        }
        else if (event.text.unicode < 128 && event.text.unicode > 31) {
            userInput += static_cast<char>(event.text.unicode);
        }
        updateText();
    }
}

bool DraggableShape::isMouseOver(const sf::Vector2i& mousePos) const {
    return shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
}

int DraggableShape::getAnchorUnderMouse(const sf::Vector2i& mousePos) const {
    for (size_t i = 0; i < anchors.size(); ++i) {
        if (anchors[i].getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
            return (int)i;
        }
    }
    return -1;
}

bool DraggableShape::isIfBlock() const {
    return shape.getFillColor() == sf::Color::Cyan;
}

int DraggableShape::getTrueAnchor() const {
    if (isIfBlock()) {
        return 1; // Right anchor index (true branch)
    }
    return -1;
}

int DraggableShape::getFalseAnchor() const {
    if (isIfBlock()) {
        return 2; // Left anchor index (false branch)
    }
    return -1;
}

sf::Vector2f DraggableShape::getAnchorPosition(int anchorIndex) const {
    if (anchorIndex < 0 || anchorIndex >= (int)anchors.size()) return { 0,0 };
    sf::FloatRect b = anchors[anchorIndex].getGlobalBounds();
    return { b.left + b.width / 2, b.top + b.height / 2 };
}

void DraggableShape::draw(sf::RenderWindow& window) const {
    window.draw(shape);
    window.draw(text);
    for (auto& a : anchors) {
        window.draw(a);
    }
}