#ifndef COD_CPP_PANEL_HPP
#define COD_CPP_PANEL_HPP

#include <SFML/Graphics.hpp>
#include <string>

struct CodCppPanel {
    sf::RectangleShape panel;
    sf::Text title;
    sf::Text codeText;

    void initialize(const sf::Font& font, float windowWidth, float windowHeight);
    void updateCode(const std::string& code);
    void draw(sf::RenderWindow& window) const;
};

#endif // COD_CPP_PANEL_HPP