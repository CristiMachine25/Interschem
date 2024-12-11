#include "DraggableShape.hpp"
#include <cmath>

DraggableShape::DraggableShape(const sf::Font& font, const std::vector<sf::Vector2f>& points, sf::Color color, const sf::Vector2f& position, bool editable)
    : userInput(""), isBeingDragged(false), isEditable(editable), isSelected(false) {
    shape.setPointCount(points.size());
    for (size_t i = 0; i < points.size(); ++i) {
        shape.setPoint(i, points[i]);
    }
    shape.setFillColor(color);
    shape.setPosition(position);

    text.setFont(font);
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::Black);
    updateTextPosition();
}

void DraggableShape::updateText() {
    text.setString(userInput);
    updateTextPosition();
}
void DraggableShape::updateTextPosition() {
    sf::FloatRect textBounds = text.getLocalBounds();
    sf::FloatRect shapeBounds = shape.getGlobalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2, textBounds.top + textBounds.height / 2);
    text.setPosition(shapeBounds.left + shapeBounds.width / 2, shapeBounds.top + shapeBounds.height / 2);

    float anchorRadius = 10.f;
    sf::Color c = shape.getFillColor();
    anchors.clear();

    if (c == sf::Color::Cyan) {
        // IF shape: 3 anchors
        anchors.resize(3, sf::CircleShape(anchorRadius));
        for (auto& a : anchors) a.setFillColor(sf::Color::Black);

        sf::Vector2f shapePos = shape.getPosition();
        sf::Vector2f topAnchorLocal(50.f, 0.f);
        sf::Vector2f rightAnchorLocal(150.f, 50.f);
        sf::Vector2f leftAnchorLocal(-50.f, 50.f);

        sf::Vector2f globalTop = shapePos + topAnchorLocal;
        sf::Vector2f globalRight = shapePos + rightAnchorLocal;
        sf::Vector2f globalLeft = shapePos + leftAnchorLocal;

        anchors[0].setPosition(globalTop.x - anchorRadius, globalTop.y - anchorRadius);
        anchors[1].setPosition(globalRight.x - anchorRadius, globalRight.y - anchorRadius);
        anchors[2].setPosition(globalLeft.x - anchorRadius, globalLeft.y - anchorRadius);
    }
    else if (c == sf::Color::Yellow) {
        // Afisare shape: 2 anchors now, above and below
        anchors.resize(2, sf::CircleShape(anchorRadius));
        for (auto& a : anchors) a.setFillColor(sf::Color::Black);

        sf::Vector2f shapePos = shape.getPosition();
        // Top anchor (100, -20)
        sf::Vector2f afisareTop(100.f, -20.f);
        // Bottom anchor (100, 120)
        sf::Vector2f afisareBottom(100.f, 120.f);

        sf::Vector2f globalTop = shapePos + afisareTop;
        sf::Vector2f globalBottom = shapePos + afisareBottom;

        anchors[0].setPosition(globalTop.x - anchorRadius, globalTop.y - anchorRadius);
        anchors[1].setPosition(globalBottom.x - anchorRadius, globalBottom.y - anchorRadius);
    }
    else if (c == sf::Color::Blue) {
        // Int shape: 2 anchors (above and below)
        anchors.resize(2, sf::CircleShape(anchorRadius));
        for (auto& a : anchors) a.setFillColor(sf::Color::Black);

        sf::Vector2f shapePos = shape.getPosition();
        sf::Vector2f globalTop = shapePos + sf::Vector2f(100.f, -20.f);
        sf::Vector2f globalBottom = shapePos + sf::Vector2f(100.f, 120.f);

        anchors[0].setPosition(globalTop.x - anchorRadius, globalTop.y - anchorRadius);
        anchors[1].setPosition(globalBottom.x - anchorRadius, globalBottom.y - anchorRadius);
    }
    else if (c == sf::Color::Green) {
        // Start shape: 1 anchor below
        anchors.resize(1, sf::CircleShape(anchorRadius));
        anchors[0].setFillColor(sf::Color::Black);

        sf::Vector2f shapePos = shape.getPosition();
        sf::Vector2f startAnchorLocal(100.f, 70.f);
        sf::Vector2f globalPos = shapePos + startAnchorLocal;
        anchors[0].setPosition(globalPos.x - anchorRadius, globalPos.y - anchorRadius);
    }
    else if (c == sf::Color::Red) {
        // Stop shape: 1 anchor above
        anchors.resize(1, sf::CircleShape(anchorRadius));
        anchors[0].setFillColor(sf::Color::Black);

        sf::Vector2f shapePos = shape.getPosition();
        sf::Vector2f stopAnchorLocal(100.f, -20.f);
        sf::Vector2f globalPos = shapePos + stopAnchorLocal;
        anchors[0].setPosition(globalPos.x - anchorRadius, globalPos.y - anchorRadius);
    }
    // Other shapes: no anchors
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