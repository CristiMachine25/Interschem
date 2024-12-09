#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <SFML/Graphics.hpp>
#include <string>

class Button {
public:
    Button(const sf::Font& font, const std::string& str, float x, float y, float width, float height);

    void updateHoverState(bool hovered);
    bool contains(const sf::Vector2f& point) const;
    void draw(sf::RenderWindow& window) const;

    sf::RectangleShape shape;
    sf::Text text;
    bool isHovered = false;

private:
    sf::Color normalColor = sf::Color(150, 50, 200);
    sf::Color hoverColor = sf::Color(170, 70, 220);
    sf::Color textColor = sf::Color::White;
};

#endif