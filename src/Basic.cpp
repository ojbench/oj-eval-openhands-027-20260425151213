#include <iostream>
#include <memory>
#include <string>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Program.hpp"
#include "Token.hpp"
#include "utils/Error.hpp"

int main() {
  Lexer lexer;
  Parser parser;
  Program program;

  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    }
    try {
      TokenStream tokens = lexer.tokenize(line);
      if (tokens.empty()) continue;

      const Token* first = tokens.peek();
      if (first->type == TokenType::RUN) {
        program.run();
      } else if (first->type == TokenType::LIST) {
        program.list();
      } else if (first->type == TokenType::CLEAR) {
        program.clear();
      } else if (first->type == TokenType::QUIT) {
        return 0;
      } else if (first->type == TokenType::HELP) {
        // Optional: print help
      } else {
        ParsedLine parsed = parser.parseLine(tokens, line);
        if (parsed.getLine().has_value()) {
          int lineNum = parsed.getLine().value();
          if (parsed.getStatement()) {
            program.addStmt(lineNum, parsed.fetchStatement());
          } else {
            program.removeStmt(lineNum);
          }
        } else {
          // Immediate execution
          if (parsed.getStatement()) {
            program.execute(parsed.getStatement());
          }
        }
      }
    } catch (const BasicError& e) {
      std::cout << e.message() << "\n";
    }
  }
  return 0;
}