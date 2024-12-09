#ifndef CONNECTIONLINE_HPP
#define CONNECTIONLINE_HPP

#include <SFML/Graphics.hpp>
#include "DraggableShape.hpp"

class ConnectionLine {
public:
    ConnectionLine(DraggableShape* s, int sa, DraggableShape* e, int ea);

    void update();
    bool involvesAnchor(DraggableShape* shape, int anchor) const;
    sf::RectangleShape line;

private:
    DraggableShape* startShape;
    int startAnchor;
    DraggableShape* endShape;
    int endAnchor;
};

#endif