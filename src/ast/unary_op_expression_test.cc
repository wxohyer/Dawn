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

#include "src/ast/unary_op_expression.h"

#include <sstream>

#include "src/ast/identifier_expression.h"
#include "src/ast/test_helper.h"

namespace tint {
namespace ast {
namespace {

using UnaryOpExpressionTest = TestHelper;

TEST_F(UnaryOpExpressionTest, Creation) {
  auto* ident = create<IdentifierExpression>("ident");

  UnaryOpExpression u(UnaryOp::kNot, ident);
  EXPECT_EQ(u.op(), UnaryOp::kNot);
  EXPECT_EQ(u.expr(), ident);
}

TEST_F(UnaryOpExpressionTest, Creation_WithSource) {
  auto* ident = create<IdentifierExpression>("ident");
  UnaryOpExpression u(Source{Source::Location{20, 2}}, UnaryOp::kNot, ident);
  auto src = u.source();
  EXPECT_EQ(src.range.begin.line, 20u);
  EXPECT_EQ(src.range.begin.column, 2u);
}

TEST_F(UnaryOpExpressionTest, IsUnaryOp) {
  UnaryOpExpression u;
  EXPECT_TRUE(u.Is<UnaryOpExpression>());
}

TEST_F(UnaryOpExpressionTest, IsValid) {
  auto* ident = create<IdentifierExpression>("ident");
  UnaryOpExpression u(UnaryOp::kNot, ident);
  EXPECT_TRUE(u.IsValid());
}

TEST_F(UnaryOpExpressionTest, IsValid_NullExpression) {
  UnaryOpExpression u;
  u.set_op(UnaryOp::kNot);
  EXPECT_FALSE(u.IsValid());
}

TEST_F(UnaryOpExpressionTest, IsValid_InvalidExpression) {
  auto* ident = create<IdentifierExpression>("");
  UnaryOpExpression u(UnaryOp::kNot, ident);
  EXPECT_FALSE(u.IsValid());
}

TEST_F(UnaryOpExpressionTest, ToStr) {
  auto* ident = create<IdentifierExpression>("ident");
  UnaryOpExpression u(UnaryOp::kNot, ident);
  std::ostringstream out;
  u.to_str(out, 2);
  EXPECT_EQ(out.str(), R"(  UnaryOp[not set]{
    not
    Identifier[not set]{ident}
  }
)");
}

}  // namespace
}  // namespace ast
}  // namespace tint
