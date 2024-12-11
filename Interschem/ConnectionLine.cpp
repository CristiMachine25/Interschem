#include "ConnectionLine.hpp"
#include <cmath>

ConnectionLine::ConnectionLine(DraggableShape* s, int sa, DraggableShape* e, int ea)
    : startShape(s), startAnchor(sa), endShape(e), endAnchor(ea) {
    sf::Color lineColor = sf::Color::Black;

    verticalLine1.setFillColor(lineColor);
    horizontalLine.setFillColor(lineColor);
    verticalLine2.setFillColor(lineColor);

    verticalLine1.setSize(sf::Vector2f(LINE_THICKNESS, 0));
    horizontalLine.setSize(sf::Vector2f(0, LINE_THICKNESS));
    verticalLine2.setSize(sf::Vector2f(LINE_THICKNESS, 0));
    //Arrowhead
    arrowhead.setPointCount(3);
    arrowhead.setFillColor(lineColor);
}

bool ConnectionLine::involvesAnchor(DraggableShape* shape, int anchor) const {
    return (shape == startShape && anchor == startAnchor) ||
        (shape == endShape && anchor == endAnchor);
}

void ConnectionLine::update() {
    sf::Vector2f startPos = startShape->getAnchorPosition(startAnchor);
    sf::Vector2f endPos = endShape->getAnchorPosition(endAnchor);

    // Dynamically detect if this connection involves an if branch
    bool isTrueOrFalseBranch = (startShape->isIfBlock() &&
        (startAnchor == startShape->getTrueAnchor() || startAnchor == startShape->getFalseAnchor())) ||
        (endShape->isIfBlock() &&
            (endAnchor == endShape->getTrueAnchor() || endAnchor == endShape->getFalseAnchor()));

    if (isTrueOrFalseBranch) {
        // Two-segment line: horizontal first, then vertical
        float horizontalGap = endPos.x - startPos.x;
        float verticalGap = endPos.y - startPos.y;

        // Adjust horizontal line
        horizontalLine.setPosition(std::min(startPos.x, endPos.x), startPos.y - LINE_THICKNESS / 2);
        horizontalLine.setSize(sf::Vector2f(std::abs(horizontalGap), LINE_THICKNESS));

        // Adjust vertical line
        verticalLine1.setPosition(endPos.x - LINE_THICKNESS / 2, std::min(startPos.y, endPos.y));
        verticalLine1.setSize(sf::Vector2f(LINE_THICKNESS, std::abs(verticalGap)));

        // Hide unused line
        verticalLine2.setSize(sf::Vector2f(0, 0));

        // Position and orient the arrowhead
        sf::Vector2f arrowBase = endPos;
        if (verticalGap > 0) { // Arrow points downward
            arrowBase.y -= 15; // Offset above the circle
            arrowhead.setRotation(0.f);
        }
        else { // Arrow points upward
            arrowBase.y += 15; // Offset below the circle
            arrowhead.setRotation(180.f);
        }
        arrowhead.setPoint(0, { 0, 0 });       // Arrow tip
        arrowhead.setPoint(1, { -15, -20 });  // Left side of V
        arrowhead.setPoint(2, { 15, -20 });   // Right side of V
        arrowhead.setPosition(arrowBase);
        arrowhead.setOutlineThickness(2.f);
        arrowhead.setOutlineColor(sf::Color::Black);
        arrowhead.setFillColor(sf::Color::Black); // Make it a V-shape
    }
    else {
        // Default three-segment line behavior
        float middleX = (startPos.x + endPos.x) / 2;
        float middleY = (startPos.y + endPos.y) / 2;

        // Ensure middleY respects a minimum vertical gap
        if (std::abs(middleY - startPos.y) < MIN_VERTICAL_GAP) {
            middleY = startPos.y + (endPos.y > startPos.y ? MIN_VERTICAL_GAP : -MIN_VERTICAL_GAP);
        }

        // Adjust vertical line 1
        verticalLine1.setPosition(startPos.x - LINE_THICKNESS / 2, std::min(startPos.y, middleY));
        verticalLine1.setSize(sf::Vector2f(LINE_THICKNESS, std::abs(middleY - startPos.y)));

        // Adjust horizontal line
        horizontalLine.setPosition(std::min(startPos.x, endPos.x) - LINE_THICKNESS / 2, middleY - LINE_THICKNESS / 2);
        horizontalLine.setSize(sf::Vector2f(std::abs(endPos.x - startPos.x), LINE_THICKNESS));

        // Adjust vertical line 2
        verticalLine2.setPosition(endPos.x - LINE_THICKNESS / 2, std::min(middleY, endPos.y));
        verticalLine2.setSize(sf::Vector2f(LINE_THICKNESS, std::abs(endPos.y - middleY)));

        // Position and orient the arrowhead
        sf::Vector2f arrowBase = endPos;
        if (endPos.y > middleY) { // Arrow points downwards
            arrowBase.y -= 15; // Offset above the circle
            arrowhead.setRotation(0.f);
        }
        else { // Arrow points upward
            arrowBase.y += 15; // Offset below the circle
            arrowhead.setRotation(180.f);
        }
        arrowhead.setPoint(0, { 0, 0 });       // Arrow tip
        arrowhead.setPoint(1, { -15, -20 });  // Left side of V
        arrowhead.setPoint(2, { 15, -20 });   // Right side of V
        arrowhead.setPosition(arrowBase);
        arrowhead.setOutlineThickness(2.f);
        arrowhead.setOutlineColor(sf::Color::Black);
        arrowhead.setFillColor(sf::Color::Black); // Make it a V-shape
    }

    // Ensure horizontal line direction is correct
    if (horizontalLine.getSize().x < 0) {
        horizontalLine.setSize(sf::Vector2f(-horizontalLine.getSize().x, LINE_THICKNESS));
        horizontalLine.setPosition(horizontalLine.getPosition().x + horizontalLine.getSize().x, horizontalLine.getPosition().y);
    }
}