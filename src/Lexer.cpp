#include "Lexer.hpp"

#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils/Error.hpp"

const std::unordered_map<std::string, TokenType> TABLE = {
    {"LET", TokenType::LET},     {"PRINT", TokenType::PRINT},
    {"INPUT", TokenType::INPUT}, {"END", TokenType::END},
    {"REM", TokenType::REM},     {"GOTO", TokenType::GOTO},
    {"IF", TokenType::IF},       {"THEN", TokenType::THEN},
    {"RUN", TokenType::RUN},     {"LIST", TokenType::LIST},
    {"CLEAR", TokenType::CLEAR}, {"QUIT", TokenType::QUIT},
    {"HELP", TokenType::HELP},  {"INDENT", TokenType::INDENT},
    {"DEDENT", TokenType::DEDENT}};

bool isOverflow(const std::string& digits, bool negative) {
  constexpr long long max_limit = std::numeric_limits<int>::max();
  return negative ? std::stol(digits) > max_limit
                  : std::stol(digits) > max_limit + 1;
}

TokenStream Lexer::tokenize(const std::string& line) const {
  std::vector<Token> tokens;
  int column = 0;
  while (column < line.size()) {
    char ch = line[column];
    if (std::isspace(static_cast<int>(ch))) {
      ++column;
      continue;
    }

    if (isLetterChar(ch)) {
      int start = column;
      ++column;
      while (column < line.size() && isLetterChar(line[column])) {
        ++column;
      }
      std::string text = line.substr(start, column - start);
      TokenType type = matchKeyword(text);
      switch (type) {
        case TokenType::REM:
          tokens.push_back(Token{TokenType::REM, text, start + 1});
          if (column < line.size()) {
            std::string comment = line.substr(column);
            tokens.push_back(Token{TokenType::REMINFO, comment, column + 1});
          }
          return TokenStream(std::move(tokens));
        case TokenType::UNKNOWN:
          tokens.push_back(Token{TokenType::IDENTIFIER, text, start + 1});
          break;
        default:
          tokens.push_back(Token{type, text, start + 1});
      }
      continue;
    }

    if (isNumberChar(ch) || (ch == '-' && column + 1 < line.size() && isNumberChar(line[column + 1]) && (tokens.empty() || (tokens.back().type != TokenType::NUMBER && tokens.back().type != TokenType::IDENTIFIER && tokens.back().type != TokenType::RIGHT_PAREN)))) {
      int start = column;
      ++column;
      while (column < line.size() && isNumberChar(line[column])) {
        ++column;
      }
      std::string text = line.substr(start, column - start);
      tokens.push_back(Token{TokenType::NUMBER, text, start + 1});
      continue;
    }

    TokenType symbolType = TokenType::UNKNOWN;
    switch (ch) {
      case '+':
        symbolType = TokenType::PLUS;
        break;
      case '-':
        symbolType = TokenType::MINUS;
        break;
      case '*':
        symbolType = TokenType::MUL;
        break;
      case '/':
        symbolType = TokenType::DIV;
        break;
      case '=':
        symbolType = TokenType::EQUAL;
        break;
      case '>':
        symbolType = TokenType::GREATER;
        break;
      case '<':
        symbolType = TokenType::LESS;
        break;
      case '(':
        symbolType = TokenType::LEFT_PAREN;
        break;
      case ')':
        symbolType = TokenType::RIGHT_PAREN;
        break;
      case ',':
        symbolType = TokenType::COMMA;
        break;
      default:
        break;
    }
    if (symbolType != TokenType::UNKNOWN) {
      tokens.push_back(Token{symbolType, std::string(1, ch), column});
      ++column;
      continue;
    }

    throw BasicError("Unexpected character '" + std::string(1, ch) +
                     "' at column " + std::to_string(column));
  }
  return TokenStream(std::move(tokens));
}

bool Lexer::isLetterChar(char ch) noexcept {
  return std::isalpha(static_cast<unsigned char>(ch)) || ch == '_';
}

bool Lexer::isNumberChar(char ch) noexcept {
  return std::isdigit(static_cast<unsigned char>(ch));
}

TokenType Lexer::matchKeyword(const std::string& text) noexcept {
  auto it = TABLE.find(text);
  if (it != TABLE.end()) {
    return it->second;
  }
  return TokenType::UNKNOWN;
}