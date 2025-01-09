#ifndef EVALUATION_HPP
#define EVALUATION_HPP
#define M_PI 3.14159265358979323846
#include <map>
#include <string>
#include <vector>
#include <cmath>

extern std::map<std::string, int> variableValues;
extern std::vector<std::string> executionOutput;  // No reference here

bool isQuotedString(const std::string& input);
std::string getQuotedText(const std::string& input);
int applyOperator(int left, int right, const std::string& op);
int applyUnaryOperator(int operand, const std::string& func);
int precedence(const std::string& op);
std::string infixToPostfix(const std::string& infix);
std::vector<std::string> convertToVec(const std::string& postfix);
int EvaluatePostfix(const std::vector<std::string>& postfix);

#endif