#include <SFML/Graphics.hpp>
#include <iostream>
#include "CodCppPanel.hpp"
#include "Button.hpp"
#include "DraggableShape.hpp"
#include "ConnectionLine.hpp"
#include "CodeGenerator.hpp"
#include <fstream>
#include <cstdlib>

void writeToFileAndCompile(const std::string& code) {
    std::ofstream outFile("generated_code.cpp");
    if (!outFile) {
        std::cerr << "Error: Could not open file for writing!" << std::endl;
        return;
    }
    outFile << code;
    outFile.close();

    std::cout << "Compiling the code..." << std::endl;
    int compileResult = system("g++ generated_code.cpp -o generated_code");

    if (compileResult != 0) {
        std::cerr << "Compilation failed!" << std::endl;
        return;
    }

    std::cout << "Execution result:" << std::endl;
    system("generated_code");
}

int main() {
    sf::RenderWindow window(sf::VideoMode(2000, 1000), "Block Interface", sf::Style::Close);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error: Could not load font.\n";
        return -1;
    }

    CodCppPanel codCppPanel;
    codCppPanel.initialize(font, window.getSize().x, window.getSize().y);
    sf::Color backgroundColor(60, 60, 60);
    sf::RectangleShape topPanel(sf::Vector2f(window.getSize().x, 50));
    topPanel.setFillColor(sf::Color(30, 30, 30));
    sf::RectangleShape leftPanel(sf::Vector2f(200, window.getSize().y));
    leftPanel.setFillColor(sf::Color(40, 40, 40));

    std::vector<std::string> buttonTexts = { "IF", "Int", "Afisare", "Start", "Stop" };
    std::vector<Button> buttons;
    float buttonWidth = 180.0f;
    float buttonHeight = 40.0f;
    float spacing = 10.0f;
    float xStart = 10.0f;
    float yStart = 60.0f;

    for (size_t i = 0; i < buttonTexts.size(); ++i) {
        buttons.emplace_back(font, buttonTexts[i], xStart, yStart + i * (buttonHeight + spacing), buttonWidth, buttonHeight);
    }

    std::vector<DraggableShape*> shapes;
    bool startExists = false;
    bool stopExists = false;

    std::vector<sf::Vector2f> trapezoidPoints = { {0, 0}, {200, 0}, {180, 100}, {20, 100} };
    std::vector<sf::Vector2f> reverseTrapezoidPoints = { {0, 0}, {200, 0}, {220, 100}, {-20, 100} };
    std::vector<sf::Vector2f> rectPoints = { {0, 0}, {200, 0}, {200, 50}, {0, 50} };
    std::vector<sf::Vector2f> ifPoints = { {0,0}, {100,0}, {150,50}, {100,100}, {0,100}, {-50,50} };

    Button rulareCodButton(font, "Run Code", 10.0f, window.getSize().y - 60.0f, 180.0f, 40.0f);
    Button* hoveredButton = nullptr;

    std::vector<ConnectionLine> lines;
    bool isDrawingLine = false;
    DraggableShape* lineStartShape = nullptr;
    int lineStartAnchor = -1;

    while (window.isOpen()) {
        sf::Event event;
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseMoved) {
                Button* newHovered = nullptr;
                for (auto& btn : buttons) {
                    if (btn.contains(static_cast<sf::Vector2f>(mousePos))) {
                        newHovered = &btn;
                        break;
                    }
                }
                if (!newHovered && rulareCodButton.contains(static_cast<sf::Vector2f>(mousePos))) {
                    newHovered = &rulareCodButton;
                }

                if (newHovered != hoveredButton) {
                    if (hoveredButton) hoveredButton->updateHoverState(false);
                    if (newHovered) newHovered->updateHoverState(true);
                    hoveredButton = newHovered;
                }

                for (auto& shape : shapes) {
                    shape->drag(mousePos);
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (!isDrawingLine) {
                    if (hoveredButton == &rulareCodButton) {
                        std::cout << "Run Code button clicked!\n";

                        // Generate code when "Run Code" button is clicked
                        std::string code = generateCode(shapes, lines);
                        codCppPanel.updateCode(code);  // Update the CodCppPanel with the generated code

                        // Write code to a file, compile, and execute
                        writeToFileAndCompile(code);
                    }
                    else if (hoveredButton) {
                        const std::string& btnText = hoveredButton->text.getString();
                        if ((btnText == "Start" && startExists) || (btnText == "Stop" && stopExists)) {
                            continue;
                        }

                        std::vector<sf::Vector2f> points;
                        sf::Color color;
                        sf::Vector2f position;
                        bool editable = true;

                        if (btnText == "IF") {
                            points = ifPoints;
                            color = sf::Color::Cyan;
                            position = { 200, 200 };
                        }
                        else if (btnText == "Int") {
                            points = trapezoidPoints;
                            color = sf::Color::Blue;
                            position = { 300, 300 };
                        }
                        else if (btnText == "Afisare") {
                            points = reverseTrapezoidPoints;
                            color = sf::Color::Yellow;
                            position = { 400, 400 };
                        }
                        else if (btnText == "Start") {
                            points = rectPoints;
                            color = sf::Color::Green;
                            position = { 500, 500 };
                            editable = false;
                            startExists = true;
                        }
                        else if (btnText == "Stop") {
                            points = rectPoints;
                            color = sf::Color::Red;
                            position = { 600, 600 };
                            editable = false;
                            stopExists = true;
                        }

                        DraggableShape* newShape = new DraggableShape(font, points, color, position, editable);
                        if (!editable) newShape->text.setString(btnText);
                        shapes.push_back(newShape);
                    }

                    bool shapeClicked = false;
                    for (auto& shape : shapes) {
                        int anchorIndex = shape->getAnchorUnderMouse(mousePos);
                        if (anchorIndex >= 0) {
                            bool lineRemoved = false;
                            for (auto it = lines.begin(); it != lines.end(); ++it) {
                                if (it->involvesAnchor(shape, anchorIndex)) {
                                    lines.erase(it);
                                    lineRemoved = true;
                                    break;
                                }
                            }
                            if (!lineRemoved) {
                                isDrawingLine = true;
                                lineStartShape = shape;
                                lineStartAnchor = anchorIndex;
                            }
                            shapeClicked = true;
                            break;
                        }
                        else if (shape->isMouseOver(mousePos)) {
                            shape->isSelected = true;
                            shape->startDrag(mousePos);
                            shapeClicked = true;
                        }
                        else {
                            shape->isSelected = false;
                        }
                    }

                    if (!shapeClicked) {
                        for (auto& shape : shapes) {
                            shape->isSelected = false;
                        }
                    }
                }
                else {
                    bool lineFinished = false;
                    for (auto& shape : shapes) {
                        int anchorIndex = shape->getAnchorUnderMouse(mousePos);
                        if (anchorIndex >= 0) {
                            ConnectionLine newLine(lineStartShape, lineStartAnchor, shape, anchorIndex);
                            lines.push_back(newLine);
                            lineFinished = true;
                            break;
                        }
                    }
                    isDrawingLine = false;
                    lineStartShape = nullptr;
                    lineStartAnchor = -1;
                }
            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                for (auto& shape : shapes) {
                    shape->stopDrag();
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                for (auto it = shapes.begin(); it != shapes.end(); ++it) {
                    DraggableShape* shape = *it;
                    if (shape->isMouseOver(mousePos)) {
                        // Remove associated lines first
                        for (auto lineIt = lines.begin(); lineIt != lines.end();) {
                            if (lineIt->involvesAnchor(shape, 0) || lineIt->involvesAnchor(shape, 1) || lineIt->involvesAnchor(shape, 2)) {
                                lineIt = lines.erase(lineIt);
                            }
                            else {
                                ++lineIt;
                            }
                        }

                        // Remove the shape itself
                        delete shape;
                        shapes.erase(it);
                        break;
                    }
                }
            }

            if (event.type == sf::Event::TextEntered) {
                for (auto& shape : shapes) {
                    if (shape->isSelected) { // Pass the text input only to the selected shape
                        shape->handleTextInput(event);
                    }
                }
            }
        }

        for (auto& line : lines) {
            line.update();
        }

        window.clear(backgroundColor);

        window.draw(topPanel);
        window.draw(leftPanel);

        for (const auto& button : buttons) {
            button.draw(window);
        }

        for (auto& shape : shapes) {
            shape->draw(window);
        }

        for (auto& line : lines) {
            if (line.verticalLine1.getSize().y > 0) {
                window.draw(line.verticalLine1);
            }
            if (line.horizontalLine.getSize().x > 0) {
                window.draw(line.horizontalLine);
            }
            if (line.verticalLine2.getSize().y > 0) {
                window.draw(line.verticalLine2);
            }
            window.draw(line.arrowhead);
        }

        rulareCodButton.draw(window);
        codCppPanel.draw(window);

        window.display();
    }

    for (auto& shape : shapes) {
        delete shape;
    }

    return 0;
}
