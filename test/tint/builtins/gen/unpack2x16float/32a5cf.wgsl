// Copyright 2021 The Tint Authors.
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

////////////////////////////////////////////////////////////////////////////////
// File generated by tools/builtin-gen
// using the template:
//   test/tint/builtins/builtins.wgsl.tmpl
// and the builtin defintion file:
//   src/tint/builtins.def
//
// Do not modify this file directly
////////////////////////////////////////////////////////////////////////////////


// fn unpack2x16float(u32) -> vec2<f32>
fn unpack2x16float_32a5cf() {
  var res: vec2<f32> = unpack2x16float(1u);
}

@stage(vertex)
fn vertex_main() -> @builtin(position) vec4<f32> {
  unpack2x16float_32a5cf();
  return vec4<f32>();
}

@stage(fragment)
fn fragment_main() {
  unpack2x16float_32a5cf();
}

@stage(compute) @workgroup_size(1)
fn compute_main() {
  unpack2x16float_32a5cf();
}
