#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>

// Struct to represent a connection point
struct ConnectionPoint {
    sf::CircleShape circle;
    sf::Vector2f offset;
    bool isConnected;

    ConnectionPoint(const sf::Vector2f& offset, float radius = 5.0f)
        : offset(offset), isConnected(false) {
        circle.setRadius(radius);
        circle.setFillColor(sf::Color::Black);
        circle.setOrigin(radius, radius);
    }
};

// Struct to represent a line between shapes
struct Line {
    sf::Vertex line[2];
    ConnectionPoint* start;
    ConnectionPoint* end;

    Line(ConnectionPoint* start, ConnectionPoint* end)
        : start(start), end(end) {
        line[0].color = sf::Color::Black;
        line[1].color = sf::Color::Black;
    }

    void update() {
        line[0].position = start->circle.getPosition();
        line[1].position = end->circle.getPosition();
    }
};

// Struct to represent a draggable shape with text input and connection points
struct DraggableShape {
    sf::ConvexShape shape;
    sf::Text text;
    std::string userInput;
    bool isBeingDragged;
    sf::Vector2f dragOffset;
    bool isEditable;
    bool isSelected;
    std::vector<ConnectionPoint> connectionPoints;

    DraggableShape(const sf::Font& font, const std::vector<sf::Vector2f>& points, sf::Color color, const sf::Vector2f& position, bool editable = true)
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

    void addConnectionPoint(const sf::Vector2f& offset) {
        ConnectionPoint cp(offset);
        updateConnectionPointPosition(cp);
        connectionPoints.push_back(cp);
    }

    void updateConnectionPoints() {
        for (auto& cp : connectionPoints) {
            updateConnectionPointPosition(cp);
        }
    }

    void updateConnectionPointPosition(ConnectionPoint& cp) {
        cp.circle.setPosition(shape.getPosition() + cp.offset);
    }

    void updateText() {
        text.setString(userInput);
        updateTextPosition();
    }

    void updateTextPosition() {
        sf::FloatRect textBounds = text.getLocalBounds();
        sf::FloatRect shapeBounds = shape.getGlobalBounds();
        text.setOrigin(textBounds.left + textBounds.width / 2, textBounds.top + textBounds.height / 2);
        text.setPosition(shapeBounds.left + shapeBounds.width / 2, shapeBounds.top + shapeBounds.height / 2);
    }

    void startDrag(const sf::Vector2i& mousePos) {
        isBeingDragged = true;
        dragOffset = shape.getPosition() - static_cast<sf::Vector2f>(mousePos);
    }

    void drag(const sf::Vector2i& mousePos) {
        if (isBeingDragged) {
            shape.setPosition(static_cast<sf::Vector2f>(mousePos) + dragOffset);
            updateTextPosition();
            updateConnectionPoints();
        }
    }

    void stopDrag() {
        isBeingDragged = false;
    }

    bool isMouseOver(const sf::Vector2i& mousePos) const {
        return shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Connection Points & Lines", sf::Style::Close);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error: Could not load font.\n";
        return -1;
    }

    // Shapes and lines
    std::vector<DraggableShape*> shapes;
    std::vector<Line> lines;
    ConnectionPoint* activeConnectionPoint = nullptr;

    // Example shapes
    DraggableShape* shape1 = new DraggableShape(font, { {0, 0}, {100, 0}, {50, 100} }, sf::Color::Blue, { 200, 200 });
    shape1->addConnectionPoint({ 50, 100 }); // Add connection point at bottom center
    shapes.push_back(shape1);

    DraggableShape* shape2 = new DraggableShape(font, { {0, 0}, {100, 0}, {50, 100} }, sf::Color::Green, { 400, 200 });
    shape2->addConnectionPoint({ 50, 100 }); // Add connection point at bottom center
    shapes.push_back(shape2);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                for (auto& shape : shapes) {
                    for (auto& cp : shape->connectionPoints) {
                        if (cp.circle.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                            if (!activeConnectionPoint) {
                                activeConnectionPoint = &cp;
                            }
                            else {
                                lines.emplace_back(activeConnectionPoint, &cp);
                                activeConnectionPoint = nullptr;
                            }
                            break;
                        }
                    }
                }
            }
        }

        for (auto& line : lines) {
            line.update();
        }

        window.clear(sf::Color::White);
        for (const auto& shape : shapes) {
            window.draw(shape->shape);
            window.draw(shape->text);
            for (const auto& cp : shape->connectionPoints) {
                window.draw(cp.circle);
            }
        }

        for (const auto& line : lines) {
            window.draw(line.line, 2, sf::Lines);
        }

        window.display();
    }

    for (auto& shape : shapes) {
        delete shape;
    }

    return 0;
}