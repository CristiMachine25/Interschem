#include "CodCppPanel.hpp"

void CodCppPanel::initialize(const sf::Font& font, float windowWidth, float windowHeight) {
    panel.setSize(sf::Vector2f(300, windowHeight)); // Panel width adjusted to 300
    panel.setFillColor(sf::Color(50, 50, 50));      // Dark gray background
    panel.setPosition(windowWidth - 300, 0);        // Positioned on the right side

    title.setFont(font);
    title.setString("Code C++");                        // Updated title
    title.setCharacterSize(30);                     // Larger font size for the title
    title.setFillColor(sf::Color::White);           // White color for title text
    title.setStyle(sf::Text::Bold | sf::Text::Underlined); // Bold and underlined
    title.setPosition(windowWidth - 290, 10);       // Adjusted position with padding

    codeText.setFont(font);
    codeText.setCharacterSize(34);                  // Larger font for code display
    codeText.setFillColor(sf::Color(200, 200, 200)); // Light gray color for code text
    codeText.setPosition(windowWidth - 290, 50);    // Positioned below the title
    codeText.setStyle(sf::Text::Regular);
}

void CodCppPanel::updateCode(const std::string& code) {
    codeText.setString(code);                       // Dynamically update displayed code
}

void CodCppPanel::draw(sf::RenderWindow& window) const {
    window.draw(panel);                             // Draw the background panel
    window.draw(title);                             // Draw the title
    window.draw(codeText);                          // Draw the code text
}