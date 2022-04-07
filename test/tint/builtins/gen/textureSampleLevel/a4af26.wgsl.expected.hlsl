Texture2DArray<float4> arg_0 : register(t0, space1);
SamplerState arg_1 : register(s1, space1);

void textureSampleLevel_a4af26() {
  float4 res = arg_0.SampleLevel(arg_1, float3(0.0f, 0.0f, float(1)), 1.0f, int2(0, 0));
}

struct tint_symbol {
  float4 value : SV_Position;
};

float4 vertex_main_inner() {
  textureSampleLevel_a4af26();
  return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

tint_symbol vertex_main() {
  const float4 inner_result = vertex_main_inner();
  tint_symbol wrapper_result = (tint_symbol)0;
  wrapper_result.value = inner_result;
  return wrapper_result;
}

void fragment_main() {
  textureSampleLevel_a4af26();
  return;
}

[numthreads(1, 1, 1)]
void compute_main() {
  textureSampleLevel_a4af26();
  return;
}