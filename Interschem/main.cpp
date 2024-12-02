#include "arb.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 600), "Parse Tree Visualization");
    sf::Font font;

    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
        return -1;
    }

    std::vector<std::string> expression = { "a", "+", "b", "*", "c" };
    Node* root = parseExpression(expression);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        drawTree(window, root, 400, 50, 150, 100, font);
        window.display();
    }

    deleteTree(root);
    return 0;
}