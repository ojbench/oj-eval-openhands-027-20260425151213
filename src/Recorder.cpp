#include "Recorder.hpp"
#include <iostream>

void Recorder::add(int line, std::unique_ptr<Statement> stmt) {
  lines_[line] = std::move(stmt);
}

void Recorder::remove(int line) {
  lines_.erase(line);
}

const Statement* Recorder::get(int line) const noexcept {
  auto it = lines_.find(line);
  if (it != lines_.end()) {
    return it->second.get();
  }
  return nullptr;
}

bool Recorder::hasLine(int line) const noexcept {
  return lines_.find(line) != lines_.end();
}

void Recorder::clear() noexcept {
  lines_.clear();
}

void Recorder::printLines() const {
  for (const auto& [line, stmt] : lines_) {
    std::cout << stmt->text() << std::endl;
  }
}

int Recorder::nextLine(int line) const noexcept {
  auto it = lines_.upper_bound(line);
  if (it != lines_.end()) {
    return it->first;
  }
  return -1;
}

int Recorder::firstLine() const noexcept {
  if (lines_.empty()) {
    return -1;
  }
  return lines_.begin()->first;
}