#include "CodeGenerator.hpp"
#include "DraggableShape.hpp"      // For DraggableShape
#include "ConnectionLine.hpp"     // For ConnectionLine
#include <sstream>                // For std::ostringstream
#include <vector>                 // For std::vector
#include <string>
#include <fstream>
#include <cstdlib>
#include <iostream>               // For std::cout and std::cerr

std::string generateCode(const std::vector<DraggableShape*>& shapes, const std::vector<ConnectionLine>& lines) {
    std::ostringstream code;

    // Add necessary includes and namespace
    code << "#include <iostream>\n";
    code << "using namespace std;\n\n";

    // Validation: Ensure there's exactly one Start and one Stop block
    DraggableShape* startBlock = nullptr;
    DraggableShape* stopBlock = nullptr;

    for (auto& shape : shapes) {
        if (shape->getFillColor() == sf::Color::Green) { // Start block
            if (startBlock) return "// Error: Multiple Start blocks found!";
            startBlock = shape;
        }
        if (shape->getFillColor() == sf::Color::Red) { // Stop block
            if (stopBlock) return "// Error: Multiple Stop blocks found!";
            stopBlock = shape;
        }
    }

    if (!startBlock) return "// Error: No Start block found!";
    if (!stopBlock) return "// Error: No Stop block found!";

    // Start code generation
    code << "int main() {\n";

    DraggableShape* currentShape = startBlock;

    // Traverse blocks following connections
    while (currentShape) {
        if (currentShape->getFillColor() == sf::Color::Blue) { // Input block
            // Check if the input block has a variable name and if it should be initialized
            if (!currentShape->userInput.empty()) {
                // Declare the variable
                code << "    int " << currentShape->userInput << ";\n";
                // Add cin for input
                code << "    cin >> " << currentShape->userInput << ";\n";
            }
        }
        else if (currentShape->getFillColor() == sf::Color::Cyan) { // If block
            code << "    if (" << currentShape->userInput << ") {\n";

            DraggableShape* trueBranch = nullptr;
            DraggableShape* falseBranch = nullptr;

            // Identify true (left) and false (right) branches
            for (auto& line : lines) {
                if (line.getStartShape() == currentShape) {
                    if (line.getStartAnchor() == currentShape->getTrueAnchor()) {
                        trueBranch = line.getEndShape(); // Left branch is True
                    }
                    else if (line.getStartAnchor() == currentShape->getFalseAnchor()) {
                        falseBranch = line.getEndShape(); // Right branch is False
                    }
                }
            }

            // Generate true branch (Left)
            if (trueBranch) {
                code << "        std::cout << " << trueBranch->userInput << ";\n";
            }

            code << "    } else {\n";

            // Generate false branch (Right)
            if (falseBranch) {
                code << "        std::cout << " << falseBranch->userInput << ";\n";
            }

            code << "    }\n";

            // Stop processing further nodes after the branches
            currentShape = nullptr;
            continue;
        }
        else if (currentShape->getFillColor() == sf::Color::Yellow) { // Output block
            code << "    std::cout << " << currentShape->userInput << ";\n";
        }
        else if (currentShape->getFillColor() == sf::Color::Red) { // Stop block
            code << "    return 0;\n"; // Add return 0 for Stop block
            break;
        }

        // Move to the next connected block
        DraggableShape* nextShape = nullptr;
        for (auto& line : lines) {
            if (line.getStartShape() == currentShape) {
                nextShape = line.getEndShape();
                break;
            }
        }

        currentShape = nextShape;
    }

    code << "}\n";
    return code.str();
}
