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

#ifndef SRC_READER_READER_H_
#define SRC_READER_READER_H_

#include <string>
#include <utility>

#include "src/ast/module.h"
#include "src/context.h"
#include "src/diagnostic/diagnostic.h"
#include "src/diagnostic/formatter.h"

namespace tint {
namespace reader {

/// Base class for input readers
class Reader {
 public:
  virtual ~Reader();

  /// Parses the input data
  /// @returns true if the parse was successful
  virtual bool Parse() = 0;

  /// @returns true if an error was encountered.
  bool has_error() const { return diags_.contains_errors(); }

  /// @returns the parser error string
  std::string error() const {
    diag::Formatter formatter{{false, false, false}};
    return formatter.format(diags_);
  }

  /// @returns the full list of diagnostic messages.
  const diag::List& diagnostics() const { return diags_; }

  /// @returns the module. The module in the parser will be reset after this.
  virtual ast::Module module() = 0;

 protected:
  /// Constructor
  /// @param ctx the context object, must be non-null
  explicit Reader(Context* ctx);

  /// Sets the diagnostic messages
  /// @param diags the list of diagnostic messages
  void set_diagnostics(const diag::List& diags) { diags_ = diags; }

  /// Creates a new `ast::Node` owned by the Context. When the Context is
  /// destructed, the `ast::Node` will also be destructed.
  /// @param args the arguments to pass to the type constructor
  /// @returns the node pointer
  template <typename T, typename... ARGS>
  T* create(ARGS&&... args) const {
    return ctx_.create<T>(std::forward<ARGS>(args)...);
  }

  /// The Tint context object
  Context& ctx_;

  /// All diagnostic messages from the reader.
  diag::List diags_;
};

}  // namespace reader
}  // namespace tint

#endif  // SRC_READER_READER_H_
