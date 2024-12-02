#ifndef ARB_H
#define ARB_H

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

struct Node {
    std::string value;
    Node* left;
    Node* right;
};

Node* createNode(const std::string& value, Node* left = nullptr, Node* right = nullptr);
Node* parseExpression(const std::vector<std::string>& tokens);
void drawTree(sf::RenderWindow& window, Node* root, float x, float y, float xOffset, float yOffset, sf::Font& font);
void deleteTree(Node* root);

#endif // ARB_H