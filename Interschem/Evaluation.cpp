#include "Evaluation.hpp"
#include <iostream>
#include <stack>
#include <sstream>
#include <cctype>
#include <cmath>
#include <map>
#include <set>

std::map<std::string, int> variableValues;

bool isQuotedString(const std::string& input) {
    return input.size() >= 2 && input[0] == '"' && input[input.size() - 1] == '"';
}

std::string getQuotedText(const std::string& input) {
    return input.substr(1, input.size() - 2);
}

// Previous operator functions remain the same
int applyOperator(int left, int right, const std::string& op) {
    if (op == "+") return left + right;
    if (op == "-") return left - right;
    if (op == "*") return left * right;
    if (op == "/") {
        if (right == 0) throw std::runtime_error("Division by zero");
        return left / right;
    }
    if (op == "<") return left < right ? 1 : 0;
    if (op == ">") return left > right ? 1 : 0;
    if (op == "==") return left == right ? 1 : 0;
    if (op == "!=") return left != right ? 1 : 0;
    if (op == "<=") return left <= right ? 1 : 0;
    if (op == ">=") return left >= right ? 1 : 0;
    if (op == "%") {
        if (right == 0) throw std::runtime_error("Modulo by zero");
        return left % right;
    }
    if (op == "^") return static_cast<int>(pow(left, right));
    throw std::runtime_error("Unknown operator: " + op);
}

int applyUnaryOperator(int operand, const std::string& func) {
    if (func == "sin") return static_cast<int>(sin(operand * M_PI / 180.0));
    if (func == "cos") return static_cast<int>(cos(operand * M_PI / 180.0));
    if (func == "sqrt") {
        if (operand < 0) throw std::runtime_error("Square root of negative number");
        return static_cast<int>(sqrt(operand));
    }
    if (func == "tg") return static_cast<int>(tan(operand * M_PI / 180.0));
    if (func == "abs") return std::abs(operand);
    if (func == "log") {
        if (operand <= 0) throw std::runtime_error("Logarithm of non-positive number");
        return static_cast<int>(log(operand));
    }
    throw std::runtime_error("Unknown function: " + func);
}

int precedence(const std::string& op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/" || op == "%") return 2;
    if (op == "^") return 3;
    if (op == "<" || op == ">" || op == "==" || op == "!=" || op == "<=" || op == ">=") return 4;
    if (op == "sin" || op == "cos" || op == "sqrt" || op == "tg" || op == "abs" || op == "log") return 5;
    return 0;
}

std::string infixToPostfix(const std::string& infix) {
    // First check if the input contains a quoted string
    size_t start_quote = infix.find('"');
    if (start_quote != std::string::npos) {
        size_t end_quote = infix.find('"', start_quote + 1);
        if (end_quote != std::string::npos) {
            // Extract the entire string including quotes
            std::string quoted_text = infix.substr(start_quote, end_quote - start_quote + 1);
            return "TEXT:" + quoted_text;
        }
    }

    // If not a quoted string, proceed with normal expression parsing
    std::stack<std::string> operators;
    std::stringstream output;
    std::string token;
    bool expectOperand = true;

    for (size_t i = 0; i < infix.length(); ++i) {
        if (std::isspace(infix[i])) continue;

        if (std::isdigit(infix[i]) || (expectOperand && infix[i] == '-')) {
            token.clear();
            if (infix[i] == '-') {
                token += infix[i++];
                if (i >= infix.length() || !std::isdigit(infix[i]))
                    throw std::runtime_error("Invalid negative number format");
            }
            while (i < infix.length() && std::isdigit(infix[i])) {
                token += infix[i++];
            }
            output << token << " ";
            i--;
            expectOperand = false;
        }
        else if (std::isalpha(infix[i])) {
            token.clear();
            while (i < infix.length() && (std::isalnum(infix[i]) || infix[i] == '_')) {
                token += infix[i++];
            }
            i--;

            if (token == "sin" || token == "cos" || token == "sqrt" || token == "tg" ||
                token == "abs" || token == "log") {
                operators.push(token);
                expectOperand = true;
            }
            else {
                output << token << " ";
                expectOperand = false;
            }
        }
        else if (infix[i] == '(') {
            operators.push("(");
            expectOperand = true;
        }
        else if (infix[i] == ')') {
            while (!operators.empty() && operators.top() != "(") {
                output << operators.top() << " ";
                operators.pop();
            }
            if (!operators.empty()) operators.pop();
            expectOperand = false;
        }
        else {
            token.clear();
            token += infix[i];
            if (i + 1 < infix.length() && (infix[i + 1] == '=' ||
                (infix[i] == '!' && infix[i + 1] == '='))) {
                token += infix[++i];
            }

            while (!operators.empty() && operators.top() != "(" &&
                precedence(operators.top()) >= precedence(token)) {
                output << operators.top() << " ";
                operators.pop();
            }
            operators.push(token);
            expectOperand = true;
        }
    }

    while (!operators.empty()) {
        if (operators.top() == "(") throw std::runtime_error("Mismatched parentheses");
        output << operators.top() << " ";
        operators.pop();
    }

    return output.str();
}

std::vector<std::string> convertToVec(const std::string& postfix) {
    std::vector<std::string> tokens;
    std::istringstream ss(postfix);
    std::string token;

    // Special handling for TEXT: prefix
    if (postfix.substr(0, 5) == "TEXT:") {
        tokens.push_back(postfix); // Keep the entire string as one token
        return tokens;
    }

    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

int EvaluatePostfix(const std::vector<std::string>& postfix) {
    if (!postfix.empty() && postfix[0].substr(0, 5) == "TEXT:") {
        std::string text = postfix[0].substr(5); // Remove the TEXT: prefix
        if (isQuotedString(text)) {
            text = getQuotedText(text);
        }
        executionOutput.push_back(text); // Add the complete text to output
        return 0;
    }

    std::stack<int> stack;

    for (const auto& token : postfix) {
        if (std::isdigit(token[0]) || (token.length() > 1 && token[0] == '-' && std::isdigit(token[1]))) {
            stack.push(std::stoi(token));
        }
        else if (std::isalpha(token[0])) {
            if (token == "sin" || token == "cos" || token == "sqrt" || token == "tg" ||
                token == "abs" || token == "log") {
                if (stack.empty())
                    throw std::runtime_error("Missing operand for function: " + token);
                int operand = stack.top();
                stack.pop();
                stack.push(applyUnaryOperator(operand, token));
            }
            else {
                auto it = variableValues.find(token);
                if (it == variableValues.end())
                    throw std::runtime_error("Undefined variable: " + token);
                stack.push(it->second);
            }
        }
        else {
            if (stack.size() < 2)
                throw std::runtime_error("Insufficient operands for operator: " + token);
            int right = stack.top();
            stack.pop();
            int left = stack.top();
            stack.pop();
            stack.push(applyOperator(left, right, token));
        }
    }

    if (stack.empty())
        throw std::runtime_error("Empty expression");
    if (stack.size() > 1)
        throw std::runtime_error("Invalid expression: too many operands");

    return stack.top();
}