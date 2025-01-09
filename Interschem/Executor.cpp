#include "Executor.hpp"
#include "ConnectionLine.hpp"
#include <iostream>

std::string executionState = "IDLE";

void setExecutionState(const std::string& newState) {
    executionState = newState;
}

std::string getExecutionState() {
    return executionState;
}

void showAlert(sf::Text& alertText, const std::string& message) {
    // A simple way to show an alert is to update the text of a Text object
    alertText.setString(message);

    // You might want to add some logic here to control how long the alert is displayed
    // and maybe add a timer or a flag to hide it later.
}

void executeBlock(Block* block, const std::vector<Block*>& allBlocks, const std::vector<ConnectionLine>& lines, sf::Text& alertText) {
    if (!block) return;

    if (block->type == "Int") {
        setExecutionState("WAITING_FOR_INPUT");
        showAlert(alertText, "Enter an integer value for 'a':");
    }
    else if (block->type == "Afisare") {
        std::string valueToDisplay = block->userInput;
        if (valueToDisplay.empty()) {
            for (const auto& line : lines) {
                if (line.endShape == block) {
                    if (line.startShape->type == "Int") {
                        valueToDisplay = line.startShape->userInput;
                        break;
                    }
                }
            }
        }
        if (valueToDisplay.empty()) valueToDisplay = "-1";
        showAlert(alertText, "Output: " + valueToDisplay);
        block->userInput = "";

        // Move to the next block
        Block* nextBlock = getNextBlock(block, lines);
        if (nextBlock) {
            executeBlock(nextBlock, allBlocks, lines, alertText);
        }
        else {
            showAlert(alertText, "Execution finished.");
            setExecutionState("FINISHED");
        }
    }
    else if (block->type == "If") {
        if (block->userInput.empty()) {
            // Check if input is needed from an Int block
            for (const auto& line : lines) {
                if (line.endShape == block && line.endShape->getAnchorUnderMouse(sf::Vector2i(line.endShape->getAnchorPosition(line.endAnchor))) == line.endAnchor) {
                    if (line.startShape->type == "Int") {
                        block->userInput = line.startShape->userInput;
                        break;
                    }
                }
            }
        }

        if (block->userInput.empty()) {
            setExecutionState("WAITING_FOR_INPUT");
            showAlert(alertText, "Enter an integer value for 'a' (for IF condition):");
        }
        else {
            // If input is already available, proceed with the next block
            Block* nextBlock = getNextBlock(block, lines);
            if (nextBlock) {
                executeBlock(nextBlock, allBlocks, lines, alertText);
            }
            else {
                showAlert(alertText, "Execution finished.");
                setExecutionState("FINISHED");
            }
        }
    }
    else if (block->type == "Stop") {
        showAlert(alertText, "Execution finished.");
        setExecutionState("FINISHED");
    }
    else {
        // Default case: move to the next block
        Block* nextBlock = getNextBlock(block, lines);
        if (nextBlock) {
            executeBlock(nextBlock, allBlocks, lines, alertText);
        }
        else {
            showAlert(alertText, "Execution finished.");
            setExecutionState("FINISHED");
        }
    }
}