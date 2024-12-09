#include "ConnectionLine.hpp"
#include <cmath>

ConnectionLine::ConnectionLine(DraggableShape* s, int sa, DraggableShape* e, int ea)
    : startShape(s), startAnchor(sa), endShape(e), endAnchor(ea) {
    line.setFillColor(sf::Color::Black);
    line.setSize(sf::Vector2f(0, 4.f));
}

void ConnectionLine::update() {
    sf::Vector2f startPos = startShape->getAnchorPosition(startAnchor);
    sf::Vector2f endPos = endShape->getAnchorPosition(endAnchor);
    sf::Vector2f diff = endPos - startPos;
    float length = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    line.setSize(sf::Vector2f(length, 4.f));

    float angle = std::atan2(diff.y, diff.x) * 180.f / 3.14159265f;
    line.setPosition(startPos);
    line.setRotation(angle);
}

bool ConnectionLine::involvesAnchor(DraggableShape* shape, int anchor) const {
    return (shape == startShape && anchor == startAnchor) ||
        (shape == endShape && anchor == endAnchor);
}