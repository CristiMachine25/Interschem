#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Block.hpp"

// Function to display an alert message (you can customize this)
void showAlert(sf::Text& alertText, const std::string& message);
void setExecutionState(const std::string& newState);
std::string getExecutionState();

// Function to execute a single block
void executeBlock(Block* block, const std::vector<Block*>& allBlocks, const std::vector<ConnectionLine>& lines, sf::Text& alertText);

#endif