#include "Statement.hpp"

#include <iostream>
#include <limits>
#include <sstream>
#include <utility>

#include "Program.hpp"
#include "VarState.hpp"
#include "utils/Error.hpp"

Statement::Statement(std::string source) : source_(std::move(source)) {}

const std::string& Statement::text() const noexcept { return source_; }

LetStatement::LetStatement(std::string source, std::string var, std::unique_ptr<Expression> expr)
    : Statement(std::move(source)), var_(std::move(var)), expr_(std::move(expr)) {}

void LetStatement::execute(VarState& state, Program& program) const {
  state.setValue(var_, expr_->evaluate(state));
}

PrintStatement::PrintStatement(std::string source, std::unique_ptr<Expression> expr)
    : Statement(std::move(source)), expr_(std::move(expr)) {}

void PrintStatement::execute(VarState& state, Program& program) const {
  std::cout << expr_->evaluate(state) << std::endl;
}

InputStatement::InputStatement(std::string source, std::string var)
    : Statement(std::move(source)), var_(std::move(var)) {}

void InputStatement::execute(VarState& state, Program& program) const {
  while (true) {
    std::cout << " ? " << std::flush;
    std::string line;
    if (!std::getline(std::cin, line)) {
      return;
    }
    if (line.empty()) continue;
    
    // Trim leading/trailing whitespace
    size_t first = line.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) continue;
    size_t last = line.find_last_not_of(" \t\r\n");
    std::string input = line.substr(first, (last - first + 1));

    try {
      size_t pos;
      int val = std::stoi(input, &pos);
      if (pos != input.length()) {
        std::cout << "INVALID NUMBER" << std::endl;
        continue;
      }
      state.setValue(var_, val);
      break;
    } catch (...) {
      std::cout << "INVALID NUMBER" << std::endl;
    }
  }
}

GotoStatement::GotoStatement(std::string source, int line)
    : Statement(std::move(source)), line_(line) {}

void GotoStatement::execute(VarState& state, Program& program) const {
  program.changePC(line_);
}

IfStatement::IfStatement(std::string source, std::unique_ptr<Expression> left, std::string op, std::unique_ptr<Expression> right, int line)
    : Statement(std::move(source)), left_(std::move(left)), op_(std::move(op)), right_(std::move(right)), line_(line) {}

void IfStatement::execute(VarState& state, Program& program) const {
  int lhs = left_->evaluate(state);
  int rhs = right_->evaluate(state);
  bool condition = false;
  if (op_ == "=") condition = (lhs == rhs);
  else if (op_ == "<") condition = (lhs < rhs);
  else if (op_ == ">") condition = (lhs > rhs);
  
  if (condition) {
    program.changePC(line_);
  }
}

EndStatement::EndStatement(std::string source) : Statement(std::move(source)) {}
void EndStatement::execute(VarState& state, Program& program) const {
  program.programEnd();
}

RemStatement::RemStatement(std::string source) : Statement(std::move(source)) {}
void RemStatement::execute(VarState& state, Program& program) const {
  // Do nothing
}

IndentStatement::IndentStatement(std::string source) : Statement(std::move(source)) {}
void IndentStatement::execute(VarState& state, Program& program) const {
  program.indent();
}

DedentStatement::DedentStatement(std::string source) : Statement(std::move(source)) {}
void DedentStatement::execute(VarState& state, Program& program) const {
  program.dedent();
}
