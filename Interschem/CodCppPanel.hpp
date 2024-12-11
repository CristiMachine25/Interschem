#ifndef COD_CPP_PANEL_HPP
#define COD_CPP_PANEL_HPP

#include <SFML/Graphics.hpp>

struct CodCppPanel {
    sf::RectangleShape panel;
    sf::Text title;

    void initialize(const sf::Font& font, float windowWidth, float windowHeight) {
        panel.setSize(sf::Vector2f(200, windowHeight));
        panel.setFillColor(sf::Color(80, 80, 80));
        panel.setPosition(windowWidth - 200, 0);

        title.setFont(font);
        title.setString("Cod C++");
        title.setCharacterSize(20);
        title.setFillColor(sf::Color::White);
        title.setPosition(windowWidth - 200 + 10, 10);
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(panel);
        window.draw(title);
    }
};

#endif // COD_CPP_PANEL_HPP