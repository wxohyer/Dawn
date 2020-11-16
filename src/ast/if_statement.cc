// Copyright 2020 The Tint Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "src/ast/if_statement.h"

#include "src/ast/else_statement.h"

namespace tint {
namespace ast {

IfStatement::IfStatement(Expression* condition, BlockStatement* body)
    : Statement(), condition_(condition), body_(body) {}

IfStatement::IfStatement(const Source& source,
                         Expression* condition,
                         BlockStatement* body)
    : Statement(source), condition_(condition), body_(body) {}

IfStatement::IfStatement(IfStatement&&) = default;

IfStatement::~IfStatement() = default;

bool IfStatement::IsIf() const {
  return true;
}

bool IfStatement::IsValid() const {
  if (condition_ == nullptr || !condition_->IsValid()) {
    return false;
  }
  if (body_ == nullptr || !body_->IsValid()) {
    return false;
  }

  bool found_else = false;
  for (auto* el : else_statements_) {
    // Else statement must be last
    if (found_else)
      return false;

    if (el == nullptr || !el->IsValid())
      return false;

    if (el->condition() == nullptr)
      found_else = true;
  }

  return true;
}

void IfStatement::to_str(std::ostream& out, size_t indent) const {
  make_indent(out, indent);
  out << "If{" << std::endl;

  // Open if conditional
  make_indent(out, indent + 2);
  out << "(" << std::endl;

  condition_->to_str(out, indent + 4);

  // Close if conditional
  make_indent(out, indent + 2);
  out << ")" << std::endl;

  // Open if body
  make_indent(out, indent + 2);
  out << "{" << std::endl;

  if (body_ != nullptr) {
    for (auto* stmt : *body_) {
      stmt->to_str(out, indent + 4);
    }
  }

  // Close the if body
  make_indent(out, indent + 2);
  out << "}" << std::endl;

  // Close the If
  make_indent(out, indent);
  out << "}" << std::endl;

  for (auto* e : else_statements_) {
    e->to_str(out, indent);
  }
}

}  // namespace ast
}  // namespace tint
