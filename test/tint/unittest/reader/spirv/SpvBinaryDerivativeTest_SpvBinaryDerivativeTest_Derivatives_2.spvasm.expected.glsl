#version 310 es
precision mediump float;

void main_1() {
  vec3 x_1 = vec3(50.0f, 60.0f, 70.0f);
  vec3 x_2 = dFdx(x_1);
  return;
}

void tint_symbol() {
  main_1();
}

void main() {
  tint_symbol();
  return;
}