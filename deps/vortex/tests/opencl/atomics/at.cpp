#define CL_TARGET_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120

#include <CL/cl.h>
#include <CL/opencl.hpp>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

const char *kernelSrc = R"CLC(
__kernel void atomic_add_test(global int* data) {
    atomic_add(&data[0], 1);
}
__kernel void atomic_sub_test(global int* data) {
    atomic_sub(&data[1], 1);
}
__kernel void atomic_inc_test(global int* data) {
    atomic_inc(&data[2]);
}
__kernel void atomic_dec_test(global int* data) {
    atomic_dec(&data[3]);
}
__kernel void atomic_min_test(global int* data) {
    int id = get_global_id(0);
    atomic_min(&data[4], id);
}
__kernel void atomic_max_test(global int* data) {
    int id = get_global_id(0);
    atomic_max(&data[5], id);
}
__kernel void atomic_and_test(global int* data) {
    int id = get_global_id(0);
    atomic_and(&data[6], ~(1 << id));
}
__kernel void atomic_or_test(global int* data) {
    int id = get_global_id(0);
    atomic_or(&data[7], (1 << id));
}
__kernel void atomic_xor_test(global int* data) {
    atomic_xor(&data[8], 1);
}
__kernel void atomic_xchg_test(global int* data) {
    int id = get_global_id(0);
    atomic_xchg(&data[9], id);
}
__kernel void atomic_cmpxchg_test(global int* data) {
    int id = get_global_id(0);
    if (id == 10) {
        atomic_cmpxchg(&data[10], 100, 999);
    }
}

__kernel void atomic_add_test_u(global uint* data) {
    atomic_add(&data[11], 1);
}
__kernel void atomic_sub_test_u(global uint* data) {
    atomic_sub(&data[12], 1);
}
__kernel void atomic_inc_test_u(global uint* data) {
    atomic_inc(&data[13]);
}
__kernel void atomic_dec_test_u(global uint* data) {
    atomic_dec(&data[14]);
}
__kernel void atomic_min_test_u(global uint* data) {
    uint id = get_global_id(0);
    atomic_min(&data[15], id);
}
__kernel void atomic_max_test_u(global uint* data) {
    uint id = get_global_id(0);
    atomic_max(&data[16], id);
}
__kernel void atomic_and_test_u(global uint* data) {
    uint id = get_global_id(0);
    atomic_and(&data[17], ~(1 << id));
}
__kernel void atomic_or_test_u(global uint* data) {
    uint id = get_global_id(0);
    atomic_or(&data[18], (1 << id));
}
__kernel void atomic_xor_test_u(global uint* data) {
    atomic_xor(&data[19], 1);
}
__kernel void atomic_xchg_test_u(global uint* data) {
    uint id = get_global_id(0);
    atomic_xchg(&data[20], id);
}
__kernel void atomic_cmpxchg_test_u(global uint* data) {
    uint id = get_global_id(0);
    if (id == 10) {
        atomic_cmpxchg(&data[21], 100, 999);
    }
}


)CLC";

void run_test(const std::string &test_name, cl::Context &context,
              cl::CommandQueue &queue, cl::Program &program, cl::Buffer &buffer,
              int N, int expected, int buffer_offset) {
  cl::Kernel kernel(program, test_name.c_str());
  kernel.setArg(0, buffer);

  queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(N),
                             cl::NullRange);
  queue.finish();

  int result = 0;
  queue.enqueueReadBuffer(buffer, CL_TRUE, sizeof(int) * buffer_offset,
                          sizeof(int), &result);
  queue.finish();

  std::cout << test_name << ":\n"
            << "  Expected: " << expected << ", Received: " << result
            << (result == expected ? " (SUCCESS)" : " (FAILURE)") << std::endl;
}

void run_test_u(const std::string &test_name, cl::Context &context,
                cl::CommandQueue &queue, cl::Program &program, cl::Buffer &buffer,
                int N, unsigned int expected, int buffer_offset) {
  cl::Kernel kernel(program, test_name.c_str());
  kernel.setArg(0, buffer);

  queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(N),
                             cl::NullRange);
  queue.finish();

  unsigned int result = 0;
  queue.enqueueReadBuffer(buffer, CL_TRUE, sizeof(unsigned int) * buffer_offset,
                          sizeof(unsigned int), &result);
  queue.finish();

  std::cout << test_name << ":\n"
            << "  Expected: " << expected << ", Received: " << result
            << (result == expected ? " (SUCCESS)" : " (FAILURE)") << std::endl;
}

int main() {
  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);
  if (platforms.empty()) {
    std::cerr << "No avaliable OpenCL platforms" << std::endl;
    return 1;
  }

  std::vector<cl::Device> devices;
  platforms[0].getDevices(CL_DEVICE_TYPE_ALL, &devices);
  if (devices.empty()) {
    std::cerr << "No devices on platform" << std::endl;
    return 2;
  }

  cl::Context context(devices[0]);
  cl::CommandQueue queue(context, devices[0]);

  size_t max_local_mem = devices[0].getInfo<CL_DEVICE_LOCAL_MEM_SIZE>();
  size_t max_work_group = devices[0].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();

  std::cout << "Device max local mem: " << max_local_mem << " bytes" << std::endl;
  std::cout << "Device max work group size: " << max_work_group << std::endl;

  const int N = 16;
  const int num_global_tests = 22;
  const int num_tests = num_global_tests;

  std::vector<int> initialValues(num_tests, 0);
  // 0: add
  initialValues[0] = 0;
  // 1: sub
  initialValues[1] = N;
  // 2: inc
  initialValues[2] = 0;
  // 3: dec
  initialValues[3] = N;
  // 4: min
  initialValues[4] = N;
  // 5: max
  initialValues[5] = -1;
  // 6: and
  initialValues[6] = -1;
  // 7: or
  initialValues[7] = 0;
  // 8: xor
  initialValues[8] = 0;
  // 9: xchg
  initialValues[9] = -1;
  // 10: cmpxchg
  initialValues[10] = 100;

  // --- Unsigned Global Init ---
  // 11: add_u
  initialValues[11] = 0;
  // 12: sub_u
  initialValues[12] = N;
  // 13: inc_u
  initialValues[13] = 0;
  // 14: dec_u
  initialValues[14] = N;
  // 15: min_u
  initialValues[15] = N;
  // 16: max_u
  initialValues[16] = 0; // unsigned min is 0
  // 17: and_u
  initialValues[17] = -1; // All bits set
  // 18: or_u
  initialValues[18] = 0;
  // 19: xor_u
  initialValues[19] = 0;
  // 20: xchg_u
  initialValues[20] = -1;
  // 21: cmpxchg_u
  initialValues[21] = 100;

  cl::Buffer buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                    sizeof(int) * num_tests, initialValues.data());

  cl::Program program(context, kernelSrc);
  cl_int build_status = program.build({devices[0]});
  if (build_status != CL_SUCCESS) {
    std::cerr << "Kernel compilation error:\n"
              << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0])
              << std::endl;
    return 3;
  }

  run_test("atomic_add_test", context, queue, program, buffer, N, N, 0);
  run_test("atomic_sub_test", context, queue, program, buffer, N, 0, 1);
  run_test("atomic_inc_test", context, queue, program, buffer, N, N, 2);
  run_test("atomic_dec_test", context, queue, program, buffer, N, 0, 3);
  run_test("atomic_min_test", context, queue, program, buffer, N, 0, 4);
  run_test("atomic_max_test", context, queue, program, buffer, N, N - 1, 5);

  int and_expected_val = ~((1 << N) - 1);
  run_test("atomic_and_test", context, queue, program, buffer, N,
           and_expected_val, 6);
  run_test("atomic_or_test", context, queue, program, buffer, N,
           (1 << N) - 1, 7);
  run_test("atomic_xor_test", context, queue, program, buffer, N, (N % 2), 8);

  cl::Kernel xchg_kernel(program, "atomic_xchg_test");
  xchg_kernel.setArg(0, buffer);
  queue.enqueueNDRangeKernel(xchg_kernel, cl::NullRange, cl::NDRange(N), cl::NullRange);
  queue.finish();

  int xchg_result = 0;
  queue.enqueueReadBuffer(buffer, CL_TRUE, sizeof(int) * 9, sizeof(int),
                          &xchg_result);
  std::cout << "atomic_xchg_test:\n"
            << "  Expected: value in [0, " << N - 1 << "], Received: " << xchg_result
            << ((xchg_result >= 0 && xchg_result < N) ? " (SUCCESS)" : " (FAILURE)") << std::endl;
  run_test("atomic_cmpxchg_test", context, queue, program, buffer, N, 999, 10);

  std::cout << "\n--- Unsigned Global Atomics Tests ---\n";
  run_test_u("atomic_add_test_u", context, queue, program, buffer, N, N, 11);
  run_test_u("atomic_sub_test_u", context, queue, program, buffer, N, 0, 12);
  run_test_u("atomic_inc_test_u", context, queue, program, buffer, N, N, 13);
  run_test_u("atomic_dec_test_u", context, queue, program, buffer, N, 0, 14);
  run_test_u("atomic_min_test_u", context, queue, program, buffer, N, 0, 15);
  run_test_u("atomic_max_test_u", context, queue, program, buffer, N, N - 1, 16);

  run_test_u("atomic_and_test_u", context, queue, program, buffer, N,
             (unsigned int)and_expected_val, 17);
  run_test_u("atomic_or_test_u", context, queue, program, buffer, N,
             (unsigned int)((1 << N) - 1), 18);
  run_test_u("atomic_xor_test_u", context, queue, program, buffer, N, (unsigned int)(N % 2), 19);

  cl::Kernel xchg_kernel_u(program, "atomic_xchg_test_u");
  xchg_kernel_u.setArg(0, buffer);
  queue.enqueueNDRangeKernel(xchg_kernel_u, cl::NullRange, cl::NDRange(N), cl::NullRange);
  queue.finish();

  unsigned int xchg_result_u = 0;
  queue.enqueueReadBuffer(buffer, CL_TRUE, sizeof(unsigned int) * 20, sizeof(unsigned int),
                          &xchg_result_u);
  std::cout << "atomic_xchg_test_u:\n"
            << "  Expected: value in [0, " << N - 1 << "], Received: " << xchg_result_u
            << ((xchg_result_u >= 0 && xchg_result_u < (unsigned int)N) ? " (SUCCESS)" : " (FAILURE)") << std::endl;

  run_test_u("atomic_cmpxchg_test_u", context, queue, program, buffer, N, 999, 21);

  return 0;
}
