#include <SFML/Graphics.hpp>
#include <iostream>
#include "CodCppPanel.hpp"
#include "Button.hpp"
#include "DraggableShape.hpp"
#include "ConnectionLine.hpp"
#include "CodeGenerator.hpp"
#include "Evaluation.hpp" // Include Evaluation.hpp
#include <fstream>
#include <cstdlib>
#include <sstream>

// Consider encapsulating these in a class (e.g., FlowchartExecutor)
std::map<std::string, int> globalVariables;
std::vector<std::string> executionOutput;
bool waitingForInput = false;
DraggableShape* waitingShape = nullptr;

void resetShapes(std::vector<DraggableShape*>& shapes) {
    for (auto* shape : shapes) {
        shape->setState(ExecutionState::NotVisited);
    }
}

// Better name: setupShapeSequence or linkShapes
void setupShapeSequence(std::vector<DraggableShape*>& shapes) {
    DraggableShape* lastIntShape = nullptr;
    DraggableShape* lastAfisareShape = nullptr;
    DraggableShape* lastIfShape = nullptr;

    for (auto* shape : shapes) {
        if (shape->getFillColor() == sf::Color::Blue) {
            std::string varName = shape->userInput;
            shape->setVariableName(varName);
            if (lastIntShape != nullptr) {
                lastIntShape->setNextShape(shape);
            }
            lastIntShape = shape;
        }
        else if (shape->getFillColor() == sf::Color::Yellow) {
            if (lastAfisareShape != nullptr) {
                lastAfisareShape->setNextShape(shape);
            }
            lastAfisareShape = shape;
        }
        else if (shape->getFillColor() == sf::Color::Cyan) {
            if (lastIfShape != nullptr) {
                lastIfShape->setNextShape(shape);
            }
            lastIfShape = shape;
        }
    }
}

// Forward declaration of the Alert struct
struct Alert;

// Declaration of the showAlert function
void showAlert(Alert& alert, const std::string& message);

void executeShape(DraggableShape* shape, std::vector<ConnectionLine>& lines, std::vector<std::string>& executionOutput, CodCppPanel& codCppPanel, Alert& alert) {
    if (shape == nullptr || shape->state == ExecutionState::Visited) {
        return;
    }

    if (shape->getFillColor() == sf::Color::Blue) { // Int block
        if (!waitingForInput) {
            std::string varName = shape->variableName;
            if (!varName.empty()) {
                codCppPanel.setInputPrompt("Enter value for " + varName + ": ");
                waitingForInput = true;
                waitingShape = shape;
                return; // Stop execution and wait for input
            }
        }
        else if (waitingForInput && waitingShape == shape) {
            int value;
            try {
                value = std::stoi(codCppPanel.getAndClearInput());
                globalVariables[shape->variableName] = value;
                waitingForInput = false;
                waitingShape = nullptr;
                shape->setState(ExecutionState::Visited);

                // Continue execution with next shape
                executeShape(shape->nextShape, lines, executionOutput, codCppPanel, alert);
                return;
            }
            catch (...) {
                showAlert(alert, "Please enter a valid number");
                return;
            }
        }
    }
    else {
        shape->setState(ExecutionState::Visited);
    }

    if (shape->getFillColor() == sf::Color::Green) { // Start block
        // Nothing to do here
    }
    else if (shape->getFillColor() == sf::Color::Yellow) { // Afisare block
        std::string expression = shape->userInput;
        std::string postfix = infixToPostfix(expression);
        std::vector<std::string> postfixTokens = convertToVec(postfix);

        // Set variable values for evaluation
        variableValues = globalVariables;

        try {
            if (!postfixTokens.empty() && postfixTokens[0].substr(0, 5) == "TEXT:") {
                // For quoted text, just evaluate (this will add to executionOutput)
                EvaluatePostfix(postfixTokens);
            }
            else {
                // For expressions, get the result and add it to output
                int result = EvaluatePostfix(postfixTokens);
                executionOutput.push_back(std::to_string(result));
            }
        }
        catch (const std::runtime_error& error) {
            showAlert(alert, error.what());
        }

        // Update the output immediately
        std::ostringstream outputStream;
        for (const auto& line : executionOutput) {
            outputStream << line << "\n";
        }
        codCppPanel.updateOutput(outputStream.str());
    }
    else if (shape->getFillColor() == sf::Color::Cyan) { // IF block
        std::string condition = shape->userInput;
        std::string postfix = infixToPostfix(condition);
        std::vector<std::string> postfixTokens = convertToVec(postfix);

        // Set variable values for evaluation
        variableValues = globalVariables;

        int result = EvaluatePostfix(postfixTokens);
        bool conditionResult = (result != 0);

        if (conditionResult) {
            executeShape(shape->trueShape, lines, executionOutput, codCppPanel, alert);
        }
        else {
            executeShape(shape->falseShape, lines, executionOutput, codCppPanel, alert);
        }
        return;
    }
    else if (shape->getFillColor() == sf::Color::Red) { // Stop block
        return;
    }

    // Continue to the next shape if not waiting for input
    if (!waitingForInput && shape->nextShape != nullptr) {
        executeShape(shape->nextShape, lines, executionOutput, codCppPanel, alert);
    }
}

struct Alert {
    sf::Text text;
    sf::RectangleShape background;
    sf::Clock clock;
    bool isVisible;
    float displayTime;
};

void initAlert(Alert& alert, const sf::Font& font) {
    alert.text.setFont(font);
    alert.text.setCharacterSize(20);
    alert.text.setFillColor(sf::Color::White);
    alert.background.setFillColor(sf::Color(50, 50, 50, 230));
    alert.isVisible = false;
    alert.displayTime = 2.0f;
}

void showAlert(Alert& alert, const std::string& message) {
    alert.text.setString(message);
    sf::FloatRect textBounds = alert.text.getLocalBounds();
    alert.text.setPosition(1000 - textBounds.width / 2, 20);
    alert.background.setSize(sf::Vector2f(textBounds.width + 20, textBounds.height + 20));
    alert.background.setPosition(alert.text.getPosition().x - 10, alert.text.getPosition().y - 10);
    alert.isVisible = true;
    alert.clock.restart();
}

void drawAlert(Alert& alert, sf::RenderWindow& window) {
    if (alert.isVisible) {
        if (alert.clock.getElapsedTime().asSeconds() < alert.displayTime) {
            window.draw(alert.background);
            window.draw(alert.text);
        }
        else {
            alert.isVisible = false;
        }
    }
}

void writeToFile(const std::string& code, Alert& alert) {
    std::ofstream outFile("generated_code.cpp");
    if (!outFile) {
        showAlert(alert, "Error: Could not open file for writing!");
        return;
    }
    outFile << code;
    outFile.close();
    showAlert(alert, "Code has been written to generated_code.cpp");
}

int main() {
    sf::RenderWindow window(sf::VideoMode(2000, 1500), "Block Interface", sf::Style::Close);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return -1;
    }

    // Initialize alert
    Alert alert;
    initAlert(alert, font);

    CodCppPanel codCppPanel;
    codCppPanel.initialize(font, window.getSize().x, window.getSize().y);

    sf::Color backgroundColor(45, 45, 45);

    sf::RectangleShape topPanel(sf::Vector2f(window.getSize().x - 400, 50));
    topPanel.setFillColor(sf::Color(30, 30, 30));

    sf::RectangleShape leftPanel(sf::Vector2f(250, window.getSize().y));
    leftPanel.setFillColor(sf::Color(35, 35, 35));

    std::vector<std::string> buttonTexts = { "IF", "Int", "Afisare", "Start", "Stop" };
    std::vector<Button> buttons;
    float buttonWidth = 220.0f;
    float buttonHeight = 45.0f;
    float spacing = 15.0f;
    float xStart = 15.0f;
    float yStart = 70.0f;

    for (size_t i = 0; i < buttonTexts.size(); ++i) {
        buttons.emplace_back(font, buttonTexts[i], xStart, yStart + i * (buttonHeight + spacing), buttonWidth, buttonHeight);
    }

    std::vector<DraggableShape*> shapes;
    bool startExists = false;
    bool stopExists = false;

    std::vector<sf::Vector2f> trapezoidPoints = { {0, 0}, {220, 0}, {200, 110}, {20, 110} };
    std::vector<sf::Vector2f> reverseTrapezoidPoints = { {0, 0}, {220, 0}, {240, 110}, {-20, 110} };
    std::vector<sf::Vector2f> rectPoints = { {0, 0}, {220, 0}, {220, 60}, {0, 60} };
    std::vector<sf::Vector2f> ifPoints = { {0,0}, {110,0}, {165,55}, {110,110}, {0,110}, {-55,55} };

    Button rulareCodButton(font, "Run Code", 15.0f, window.getSize().y - 70.0f, 220.0f, 45.0f);
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
                        // 1. Generate code
                        std::string code = generateCode(shapes, lines);

                        // 2. Update CodCppPanel with generated code
                        codCppPanel.updateCode(code);

                        // 3. Draw the window to show the generated code
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
                        drawAlert(alert, window);
                        window.display(); // Display the generated code

                        // 4. Write code to file (you can do this before or after displaying)
                        writeToFile(code, alert);

                        // 5. Reset and prepare for execution
                        globalVariables.clear();
                        resetShapes(shapes);
                        setupShapeSequence(shapes);

                        // 6. Find Start block
                        DraggableShape* startShape = nullptr;
                        for (auto& shape : shapes) {
                            if (shape->getFillColor() == sf::Color::Green) {
                                startShape = shape;
                                break;
                            }
                        }

                        // 7. Execute flowchart (if Start block is found)
                        if (startShape) {
                            // Clear previous output
                            executionOutput.clear();

                            // Execute the flowchart
                            executeShape(startShape, lines, executionOutput, codCppPanel, alert);
                        }
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
                            position = { 300, 200 };
                        }
                        else if (btnText == "Int") {
                            points = trapezoidPoints;
                            color = sf::Color::Blue;
                            position = { 400, 300 };
                        }
                        else if (btnText == "Afisare") {
                            points = reverseTrapezoidPoints;
                            color = sf::Color::Yellow;
                            position = { 500, 400 };
                        }
                        else if (btnText == "Start") {
                            points = rectPoints;
                            color = sf::Color::Green;
                            position = { 300, 100 };
                            editable = false;
                            startExists = true;
                        }
                        else if (btnText == "Stop") {
                            points = rectPoints;
                            color = sf::Color::Red;
                            position = { 300, 600 };
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
                        for (auto lineIt = lines.begin(); lineIt != lines.end();) {
                            if (lineIt->involvesAnchor(shape, 0) || lineIt->involvesAnchor(shape, 1) ||
                                lineIt->involvesAnchor(shape, 2)) {
                                lineIt = lines.erase(lineIt);
                            }
                            else {
                                ++lineIt;
                            }
                        }

                        if (shape->text.getString() == "Start") startExists = false;
                        if (shape->text.getString() == "Stop") stopExists = false;

                        delete shape;
                        shapes.erase(it);
                        break;
                    }
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Enter) {
                    if (waitingForInput) {
                        // Find Start block
                        DraggableShape* startShape = nullptr;
                        for (auto& shape : shapes) {
                            if (shape->getFillColor() == sf::Color::Green) {
                                startShape = shape;
                                break;
                            }
                        }

                        // Continue execution from the waiting shape
                        if (startShape) {
                            executeShape(waitingShape, lines, executionOutput, codCppPanel, alert);

                            // Update output display
                            std::ostringstream outputStream;
                            for (const auto& line : executionOutput) {
                                outputStream << line << "\n";
                            }
                            codCppPanel.updateOutput(outputStream.str());
                        }
                    }
                }
            }

            if (event.type == sf::Event::TextEntered) {
                for (auto& shape : shapes) {
                    if (shape->isSelected) {
                        shape->handleTextInput(event);
                    }
                }
                codCppPanel.handleTextInput(event);
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
        drawAlert(alert, window);
        window.display();
    }

    for (auto& shape : shapes) {
        delete shape;
    }
    shapes.clear();

    return 0;
}