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

#include <memory>

#include "src/ast/assignment_statement.h"
#include "src/ast/discard_statement.h"
#include "src/ast/float_literal.h"
#include "src/ast/identifier_expression.h"
#include "src/ast/loop_statement.h"
#include "src/ast/module.h"
#include "src/ast/return_statement.h"
#include "src/ast/type/f32_type.h"
#include "src/ast/variable.h"
#include "src/ast/variable_decl_statement.h"
#include "src/writer/hlsl/test_helper.h"

namespace tint {
namespace writer {
namespace hlsl {
namespace {

using HlslGeneratorImplTest_Loop = TestHelper;

TEST_F(HlslGeneratorImplTest_Loop, Emit_Loop) {
  auto* body = create<ast::BlockStatement>();
  body->append(create<ast::DiscardStatement>());

  ast::LoopStatement l(std::move(body), {});
  gen.increment_indent();

  ASSERT_TRUE(gen.EmitStatement(out, &l)) << gen.error();
  EXPECT_EQ(result(), R"(  for(;;) {
    discard;
  }
)");
}

TEST_F(HlslGeneratorImplTest_Loop, Emit_LoopWithContinuing) {
  auto* body = create<ast::BlockStatement>();
  body->append(create<ast::DiscardStatement>());

  auto* continuing = create<ast::BlockStatement>();
  continuing->append(create<ast::ReturnStatement>());

  ast::LoopStatement l(std::move(body), std::move(continuing));
  gen.increment_indent();

  ASSERT_TRUE(gen.EmitStatement(out, &l)) << gen.error();
  EXPECT_EQ(result(), R"(  {
    bool tint_hlsl_is_first_1 = true;
    for(;;) {
      if (!tint_hlsl_is_first_1) {
        return;
      }
      tint_hlsl_is_first_1 = false;

      discard;
    }
  }
)");
}

TEST_F(HlslGeneratorImplTest_Loop, Emit_LoopNestedWithContinuing) {
  ast::type::F32Type f32;

  auto* body = create<ast::BlockStatement>();
  body->append(create<ast::DiscardStatement>());

  auto* continuing = create<ast::BlockStatement>();
  continuing->append(create<ast::ReturnStatement>());

  auto* inner =
      create<ast::LoopStatement>(std::move(body), std::move(continuing));

  body = create<ast::BlockStatement>();
  body->append(std::move(inner));

  auto* lhs = create<ast::IdentifierExpression>("lhs");
  auto* rhs = create<ast::IdentifierExpression>("rhs");

  continuing = create<ast::BlockStatement>();
  continuing->append(
      create<ast::AssignmentStatement>(std::move(lhs), std::move(rhs)));

  ast::LoopStatement outer(std::move(body), std::move(continuing));
  gen.increment_indent();

  ASSERT_TRUE(gen.EmitStatement(out, &outer)) << gen.error();
  EXPECT_EQ(result(), R"(  {
    bool tint_hlsl_is_first_1 = true;
    for(;;) {
      if (!tint_hlsl_is_first_1) {
        lhs = rhs;
      }
      tint_hlsl_is_first_1 = false;

      {
        bool tint_hlsl_is_first_2 = true;
        for(;;) {
          if (!tint_hlsl_is_first_2) {
            return;
          }
          tint_hlsl_is_first_2 = false;

          discard;
        }
      }
    }
  }
)");
}

TEST_F(HlslGeneratorImplTest_Loop, Emit_LoopWithVarUsedInContinuing) {
  // loop {
  //   var lhs : f32 = 2.4;
  //   var other : f32;
  //   continuing {
  //     lhs = rhs
  //   }
  // }
  //
  // ->
  // {
  //   float lhs;
  //   float other;
  //   for (;;) {
  //     if (continuing) {
  //       lhs = rhs;
  //     }
  //     lhs = 2.4f;
  //     other = 0.0f;
  //   }
  // }

  ast::type::F32Type f32;

  auto* var = create<ast::Variable>("lhs", ast::StorageClass::kFunction, &f32);
  var->set_constructor(create<ast::ScalarConstructorExpression>(
      create<ast::FloatLiteral>(&f32, 2.4)));

  auto* body = create<ast::BlockStatement>();
  body->append(create<ast::VariableDeclStatement>(std::move(var)));
  body->append(create<ast::VariableDeclStatement>(
      create<ast::Variable>("other", ast::StorageClass::kFunction, &f32)));

  auto* lhs = create<ast::IdentifierExpression>("lhs");
  auto* rhs = create<ast::IdentifierExpression>("rhs");

  auto* continuing = create<ast::BlockStatement>();
  continuing->append(
      create<ast::AssignmentStatement>(std::move(lhs), std::move(rhs)));

  ast::LoopStatement outer(std::move(body), std::move(continuing));
  gen.increment_indent();

  ASSERT_TRUE(gen.EmitStatement(out, &outer)) << gen.error();
  EXPECT_EQ(result(), R"(  {
    bool tint_hlsl_is_first_1 = true;
    float lhs;
    float other;
    for(;;) {
      if (!tint_hlsl_is_first_1) {
        lhs = rhs;
      }
      tint_hlsl_is_first_1 = false;

      lhs = 2.40000010f;
      other = 0.0f;
    }
  }
)");
}

}  // namespace
}  // namespace hlsl
}  // namespace writer
}  // namespace tint
