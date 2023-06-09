struct Interface {
  @location(1)
  col1 : f32,
  @location(2)
  col2 : f32,
  @builtin(position)
  pos : vec4<f32>,
}

@vertex
fn vert_main() -> Interface {
  return Interface(0.4000000000000000222, 0.5999999999999999778, vec4<f32>());
}

@fragment
fn frag_main(colors : Interface) {
  let r : f32 = colors.col1;
  let g : f32 = colors.col2;
}
