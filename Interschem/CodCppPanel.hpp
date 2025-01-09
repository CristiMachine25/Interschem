// CodCppPanel.hpp
#ifndef COD_CPP_PANEL_HPP
#define COD_CPP_PANEL_HPP

#include <SFML/Graphics.hpp>
#include <string>

struct CodCppPanel {
    sf::RectangleShape panel;
    sf::Text title;
    sf::Text codeText;
    sf::Text outputText;
    sf::Text outputTitle;
    sf::Text inputTitle;            // New: Title for input section
    sf::Text inputPrompt;           // New: Shows what input is needed
    sf::Text currentInput;          // New: Shows what user is typing
    sf::RectangleShape inputBox;    // New: Visual box for input
    bool isInputActive;             // New: Track if we're accepting input
    std::string pendingInput;       // New: Store current input
    const sf::Font* font;

    void initialize(const sf::Font& font, float windowWidth, float windowHeight);
    void updateCode(const std::string& code);
    void updateOutput(const std::string& output);
    void draw(sf::RenderWindow& window) const;
    void clearOutput();
    void setInputPrompt(const std::string& prompt);    // New: Update input prompt
    void handleTextInput(sf::Event event);             // New: Handle text input
    std::string getAndClearInput();                    // New: Get and clear current input
};

#endif // COD_CPP_PANEL_HPP