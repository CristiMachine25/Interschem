#include "Button.hpp"

Button::Button(const sf::Font& font, const std::string& str, float x, float y, float width, float height) {
    shape.setSize(sf::Vector2f(width, height));
    shape.setFillColor(normalColor);
    shape.setPosition(x, y);
    shape.setOutlineThickness(2.0f);
    shape.setOutlineColor(sf::Color::Black);

    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(24);
    text.setFillColor(textColor);

    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2, textBounds.top + textBounds.height / 2);
    text.setPosition(x + width / 2, y + height / 2);
}

void Button::updateHoverState(bool hovered) {
    isHovered = hovered;
    shape.setFillColor(hovered ? hoverColor : normalColor);
}

bool Button::contains(const sf::Vector2f& point) const {
    return shape.getGlobalBounds().contains(point);
}

void Button::draw(sf::RenderWindow& window) const {
    window.draw(shape);
    window.draw(text);
}