#ifndef CONNECTIONLINE_HPP
#define CONNECTIONLINE_HPP

#include <SFML/Graphics.hpp>
#include "DraggableShape.hpp"

struct ConnectionLine {
    ConnectionLine(DraggableShape* s, int sa, DraggableShape* e, int ea);

    void update();
    bool involvesAnchor(DraggableShape* shape, int anchor) const;

    sf::RectangleShape verticalLine1;
    sf::RectangleShape horizontalLine;
    sf::RectangleShape verticalLine2;
    sf::ConvexShape arrowhead; // New arrowhead

    static constexpr float LINE_THICKNESS = 4.f;
    static constexpr float MIN_VERTICAL_GAP = 30.f;

    DraggableShape* startShape;
    int startAnchor;
    DraggableShape* endShape;
    int endAnchor;

    int getStartAnchor() const { return startAnchor; }
    int getEndAnchor() const { return endAnchor; }
    DraggableShape* getStartShape() const { return startShape; }
    DraggableShape* getEndShape() const { return endShape; }
};

#endif