var<private> local_invocation_index_1 : u32;

var<workgroup> arg_0 : atomic<u32>;

fn atomicOr_5e3d61() {
  var res : u32 = 0u;
  let x_10 : u32 = atomicOr(&(arg_0), 1u);
  res = x_10;
  return;
}

fn compute_main_inner(local_invocation_index_2 : u32) {
  atomicStore(&(arg_0), 0u);
  workgroupBarrier();
  atomicOr_5e3d61();
  return;
}

fn compute_main_1() {
  let x_30 : u32 = local_invocation_index_1;
  compute_main_inner(x_30);
  return;
}

@compute @workgroup_size(1i, 1i, 1i)
fn compute_main(@builtin(local_invocation_index) local_invocation_index_1_param : u32) {
  local_invocation_index_1 = local_invocation_index_1_param;
  compute_main_1();
}
