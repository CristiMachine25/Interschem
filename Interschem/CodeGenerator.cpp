#include "CodeGenerator.hpp"
#include "DraggableShape.hpp"
#include "ConnectionLine.hpp"
#include <sstream>
#include <vector>
#include <string>

void generateBranchCode(std::ostringstream& code, DraggableShape* currentShape, const std::vector<ConnectionLine>& lines, int indentLevel, bool& hasReturn) {
    std::string indent(indentLevel * 4, ' ');

    while (currentShape) {
        if (currentShape->getFillColor() == sf::Color::Yellow) { // Afisare block
            code << indent << "std::cout << " << currentShape->userInput << ";\n";
        }
        else if (currentShape->getFillColor() == sf::Color::Cyan) { // If block
            code << indent << "if (" << currentShape->userInput << ") {\n";
            DraggableShape* trueBranch = nullptr;
            DraggableShape* falseBranch = nullptr;

            for (const auto& line : lines) {
                if (line.getStartShape() == currentShape) {
                    if (line.getStartAnchor() == currentShape->getFalseAnchor()) { // Index 2 is for TRUE
                        trueBranch = line.getEndShape();
                    }
                    else if (line.getStartAnchor() == currentShape->getTrueAnchor()) { // Index 1 is for FALSE
                        falseBranch = line.getEndShape();
                    }
                }
            }

            generateBranchCode(code, trueBranch, lines, indentLevel + 1, hasReturn); // Generate code for TRUE branch
            code << indent << "}";

            if (falseBranch) {
                code << " else {\n";
                generateBranchCode(code, falseBranch, lines, indentLevel + 1, hasReturn); // Generate code for FALSE branch
                code << indent << "}";
            }
            code << "\n";
            return; // Stop further processing of the current branch
        }
        else if (currentShape->getFillColor() == sf::Color::Red) { // Stop block
            code << indent << "return 0;\n";
            hasReturn = true;
            return; // Stop processing after encountering a Stop block
        }
        else if (currentShape->getFillColor() == sf::Color::Blue) { // Int block
            code << indent << "int " << currentShape->userInput << ";\n";
            code << indent << "std::cin >> " << currentShape->userInput << ";\n";
        }

        // Find the next shape in the sequence (for non-IF blocks)
        DraggableShape* nextShape = nullptr;
        for (const auto& line : lines) {
            if (line.getStartShape() == currentShape && !currentShape->isIfBlock()) {
                nextShape = line.getEndShape();
                break;
            }
        }
        currentShape = nextShape;
    }
}

std::string generateCode(const std::vector<DraggableShape*>& shapes, const std::vector<ConnectionLine>& lines) {
    std::ostringstream code;

    // Reset the nextShape, trueShape, and falseShape pointers
    for (auto* shape : shapes) {
        shape->setNextShape(nullptr);
        shape->setTrueShape(nullptr);
        shape->setFalseShape(nullptr);
    }

    // Build the connections between shapes
    for (const auto& line : lines) {
        DraggableShape* startShape = line.getStartShape();
        DraggableShape* endShape = line.getEndShape();
        int startAnchor = line.getStartAnchor();

        if (startShape->isIfBlock()) {
            if (startAnchor == startShape->getFalseAnchor()) { // Index 2 is for TRUE
                startShape->setTrueShape(endShape);
            }
            else if (startAnchor == startShape->getTrueAnchor()) { // Index 1 is for FALSE
                startShape->setFalseShape(endShape);
            }
        }
        else {
            startShape->setNextShape(endShape);
        }
    }

    // Find the Start block
    DraggableShape* startBlock = nullptr;
    for (auto* shape : shapes) {
        if (shape->getFillColor() == sf::Color::Green) {
            startBlock = shape;
            break;
        }
    }

    if (!startBlock) {
        return "// Error: No Start block found!";
    }

    code << "#include <iostream>\n";
    code << "int main() {\n";

    bool hasReturn = false; // Flag to track if a return statement has been generated
    generateBranchCode(code, startBlock, lines, 1, hasReturn);

    // Add return 0; at the end if no Stop block was encountered
    if (!hasReturn) {
        code << "    return 0;\n";
    }

    code << "}\n";

    return code.str();

    return code.str();
}