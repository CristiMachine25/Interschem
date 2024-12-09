#include "arb.h"
#include <stack>
#include <cmath>

Node* createNode(const std::string& value, Node* left, Node* right) {
    return new Node{ value, left, right };
}

int precedence(const std::string& op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    return 0;
}

bool isOperator(const std::string& token) {
    return token == "+" || token == "-" || token == "*" || token == "/";
}

Node* parseExpression(const std::vector<std::string>& tokens) {
    std::stack<Node*> values;
    std::stack<std::string> ops;

    for (const std::string& token : tokens) {
        if (isalnum(token[0])) {
            values.push(createNode(token));
        }
        else if (token == "(") {
            ops.push(token);
        }
        else if (token == ")") {
            while (!ops.empty() && ops.top() != "(") {
                std::string op = ops.top(); ops.pop();
                Node* right = values.top(); values.pop();
                Node* left = values.top(); values.pop();
                values.push(createNode(op, left, right));
            }
            ops.pop();
        }
        else if (isOperator(token)) {
            while (!ops.empty() && precedence(ops.top()) >= precedence(token)) {
                std::string op = ops.top(); ops.pop();
                Node* right = values.top(); values.pop();
                Node* left = values.top(); values.pop();
                values.push(createNode(op, left, right));
            }
            ops.push(token);
        }
    }

    while (!ops.empty()) {
        std::string op = ops.top(); ops.pop();
        Node* right = values.top(); values.pop();
        Node* left = values.top(); values.pop();
        values.push(createNode(op, left, right));
    }

    return values.top();
}

void drawTree(sf::RenderWindow& window, Node* root, float x, float y, float xOffset, float yOffset, sf::Font& font) {
    if (root == nullptr) return;

    sf::CircleShape circle(20);
    circle.setFillColor(sf::Color::Blue);
    circle.setPosition(x - 20, y - 20);

    sf::Text text(root->value, font, 20);
    text.setFillColor(sf::Color::White);
    text.setPosition(x - 10, y - 15);

    if (root->left) {
        sf::VertexArray line(sf::Lines, 2);
        line[0].position = sf::Vector2f(x, y);
        line[1].position = sf::Vector2f(x - xOffset, y + yOffset);
        line[0].color = line[1].color = sf::Color::White;
        window.draw(line);
        drawTree(window, root->left, x - xOffset, y + yOffset, xOffset / 2, yOffset, font);
    }

    if (root->right) {
        sf::VertexArray line(sf::Lines, 2);
        line[0].position = sf::Vector2f(x, y);
        line[1].position = sf::Vector2f(x + xOffset, y + yOffset);
        line[0].color = line[1].color = sf::Color::White;
        window.draw(line);
        drawTree(window, root->right, x + xOffset, y + yOffset, xOffset / 2, yOffset, font);
    }

    window.draw(circle);
    window.draw(text);
}

void deleteTree(Node* root) {
    if (root == nullptr) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}