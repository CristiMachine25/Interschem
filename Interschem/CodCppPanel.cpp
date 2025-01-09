// CodCppPanel.cpp
#include "CodCppPanel.hpp"

void CodCppPanel::initialize(const sf::Font& font, float windowWidth, float windowHeight) {
    this->font = &font;
    panel.setSize(sf::Vector2f(400, windowHeight));
    panel.setFillColor(sf::Color(50, 50, 50));
    panel.setPosition(windowWidth - 400, 0);

    title.setFont(font);
    title.setString("Code C++");
    title.setCharacterSize(30);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold | sf::Text::Underlined);
    title.setPosition(windowWidth - 390, 10);

    codeText.setFont(font);
    codeText.setCharacterSize(24);
    codeText.setFillColor(sf::Color(200, 200, 200));
    codeText.setPosition(windowWidth - 390, 50);
    codeText.setStyle(sf::Text::Regular);

    // Initialize input section
    inputTitle.setFont(font);
    inputTitle.setString("Input:");
    inputTitle.setCharacterSize(26);
    inputTitle.setFillColor(sf::Color::White);
    inputTitle.setStyle(sf::Text::Bold);
    inputTitle.setPosition(windowWidth - 390, windowHeight / 2 - 140);

    inputPrompt.setFont(font);
    inputPrompt.setString("");
    inputPrompt.setCharacterSize(24);
    inputPrompt.setFillColor(sf::Color::Yellow);
    inputPrompt.setPosition(windowWidth - 390, windowHeight / 2 - 100);

    currentInput.setFont(font);
    currentInput.setString("");
    currentInput.setCharacterSize(24);
    currentInput.setFillColor(sf::Color::White);
    currentInput.setPosition(windowWidth - 390, windowHeight / 2 - 60);

    inputBox.setSize(sf::Vector2f(380, 40));
    inputBox.setFillColor(sf::Color(30, 30, 30));
    inputBox.setPosition(windowWidth - 390, windowHeight / 2 - 70);
    inputBox.setOutlineColor(sf::Color::White);
    inputBox.setOutlineThickness(1);

    // Initialize output section (moved down)
    outputTitle.setFont(font);
    outputTitle.setString("Output:");
    outputTitle.setCharacterSize(26);
    outputTitle.setFillColor(sf::Color::White);
    outputTitle.setStyle(sf::Text::Bold);
    outputTitle.setPosition(windowWidth - 390, windowHeight / 2);

    outputText.setFont(font);
    outputText.setCharacterSize(24);
    outputText.setFillColor(sf::Color::Green);
    outputText.setPosition(windowWidth - 390, windowHeight / 2 + 50);
    outputText.setStyle(sf::Text::Regular);

    isInputActive = false;
    pendingInput = "";
}

void CodCppPanel::setInputPrompt(const std::string& prompt) {
    inputPrompt.setString(prompt);
    isInputActive = true;
    pendingInput = "";
    currentInput.setString("");
}

void CodCppPanel::handleTextInput(sf::Event event) {
    if (!isInputActive) return;

    if (event.text.unicode == '\b') { // Backspace
        if (!pendingInput.empty()) {
            pendingInput.pop_back();
        }
    }
    // Don't handle Enter here anymore since we're handling it in KeyPressed event
    else if (event.text.unicode >= 32 && event.text.unicode < 128) {
        pendingInput += static_cast<char>(event.text.unicode);
    }
    currentInput.setString(pendingInput);
}

std::string CodCppPanel::getAndClearInput() {
    isInputActive = false;
    std::string input = pendingInput;
    pendingInput = "";
    currentInput.setString("");
    return input;
}

void CodCppPanel::updateCode(const std::string& code) {
    codeText.setString(code);
}

void CodCppPanel::updateOutput(const std::string& output) {
    outputText.setString(output);
}

void CodCppPanel::draw(sf::RenderWindow& window) const {
    window.draw(panel);
    window.draw(title);
    window.draw(codeText);
    window.draw(inputTitle);
    window.draw(inputBox);
    window.draw(inputPrompt);
    window.draw(currentInput);
    window.draw(outputTitle);
    window.draw(outputText);
}