#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <vector>
#include <string>
#include <sstream>
#include "DraggableShape.hpp"
#include "ConnectionLine.hpp"

std::string generateCode(const std::vector<DraggableShape*>& shapes, const std::vector<ConnectionLine>& lines);

#endif // CODE_GENERATOR_HPP