// Copyright 2021 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_CCTEST_TEST_HELPER_RISCV_H_
#define V8_CCTEST_TEST_HELPER_RISCV_H_

#include "src/codegen/assembler-inl.h"
#include "src/codegen/macro-assembler.h"
#include "src/diagnostics/disassembler.h"
#include "src/execution/simulator.h"
#include "src/heap/factory.h"
#include "src/init/v8.h"
#include "src/utils/utils.h"
#include "test/cctest/cctest.h"

#define PRINT_RES(res, expected_res, in_hex)                         \
  if (in_hex) std::cout << "[hex-form]" << std::hex;                 \
  std::cout << "res = " << (res) << " expected = " << (expected_res) \
            << std::endl;

namespace v8 {
namespace internal {

using Func = std::function<void(MacroAssembler&)>;

int32_t GenAndRunTest(Func test_generator);

// f.Call(...) interface is implemented as varargs in V8. For varargs,
// floating-point arguments and return values are passed in GPRs, therefore
// the special handling to reinterpret floating-point as integer values when
// passed in and out of f.Call()
template <typename OUTPUT_T, typename INPUT_T>
OUTPUT_T GenAndRunTest(INPUT_T input0, Func test_generator) {
  DCHECK((sizeof(INPUT_T) == 4 || sizeof(INPUT_T) == 8));

  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, v8::internal::CodeObjectRequired::kYes);

  // handle floating-point parameters
  if (std::is_same<float, INPUT_T>::value) {
    assm.fmv_w_x(fa0, a0);
  } else if (std::is_same<double, INPUT_T>::value) {
    assm.fmv_d_x(fa0, a0);
  }

  test_generator(assm);

  // handle floating-point result
  if (std::is_same<float, OUTPUT_T>::value) {
    assm.fmv_x_w(a0, fa0);
  } else if (std::is_same<double, OUTPUT_T>::value) {
    assm.fmv_x_d(a0, fa0);
  }
  assm.jr(ra);

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code =
      Factory::CodeBuilder(isolate, desc, CodeKind::FOR_TESTING).Build();

  using OINT_T = typename std::conditional<
      std::is_integral<OUTPUT_T>::value, OUTPUT_T,
      typename std::conditional<sizeof(OUTPUT_T) == 4, int32_t,
                                int64_t>::type>::type;
  using IINT_T = typename std::conditional<
      std::is_integral<INPUT_T>::value, INPUT_T,
      typename std::conditional<sizeof(INPUT_T) == 4, int32_t,
                                int64_t>::type>::type;

  auto f = GeneratedCode<OINT_T(IINT_T)>::FromCode(*code);

  auto res = f.Call(bit_cast<IINT_T>(input0));
  return bit_cast<OUTPUT_T>(res);
}

template <typename OUTPUT_T, typename INPUT_T>
OUTPUT_T GenAndRunTest(INPUT_T input0, INPUT_T input1, Func test_generator) {
  DCHECK((sizeof(INPUT_T) == 4 || sizeof(INPUT_T) == 8));

  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, v8::internal::CodeObjectRequired::kYes);

  // handle floating-point parameters
  if (std::is_same<float, INPUT_T>::value) {
    assm.fmv_w_x(fa0, a0);
    assm.fmv_w_x(fa1, a1);
  } else if (std::is_same<double, INPUT_T>::value) {
    assm.fmv_d_x(fa0, a0);
    assm.fmv_d_x(fa1, a1);
  }

  test_generator(assm);

  // handle floating-point result
  if (std::is_same<float, OUTPUT_T>::value) {
    assm.fmv_x_w(a0, fa0);
  } else if (std::is_same<double, OUTPUT_T>::value) {
    assm.fmv_x_d(a0, fa0);
  }
  assm.jr(ra);

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code =
      Factory::CodeBuilder(isolate, desc, CodeKind::FOR_TESTING).Build();

  using OINT_T = typename std::conditional<
      std::is_integral<OUTPUT_T>::value, OUTPUT_T,
      typename std::conditional<sizeof(OUTPUT_T) == 4, int32_t,
                                int64_t>::type>::type;
  using IINT_T = typename std::conditional<
      std::is_integral<INPUT_T>::value, INPUT_T,
      typename std::conditional<sizeof(INPUT_T) == 4, int32_t,
                                int64_t>::type>::type;
  auto f = GeneratedCode<OINT_T(IINT_T, IINT_T)>::FromCode(*code);

  auto res = f.Call(bit_cast<IINT_T>(input0), bit_cast<IINT_T>(input1));
  return bit_cast<OUTPUT_T>(res);
}

template <typename OUTPUT_T, typename INPUT_T>
OUTPUT_T GenAndRunTest(INPUT_T input0, INPUT_T input1, INPUT_T input2,
                       Func test_generator) {
  DCHECK((sizeof(INPUT_T) == 4 || sizeof(INPUT_T) == 8));

  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, v8::internal::CodeObjectRequired::kYes);

  // handle floating-point parameters
  if (std::is_same<float, INPUT_T>::value) {
    assm.fmv_w_x(fa0, a0);
    assm.fmv_w_x(fa1, a1);
    assm.fmv_w_x(fa2, a2);
  } else if (std::is_same<double, INPUT_T>::value) {
    assm.fmv_d_x(fa0, a0);
    assm.fmv_d_x(fa1, a1);
    assm.fmv_d_x(fa2, a2);
  }

  test_generator(assm);

  // handle floating-point result
  if (std::is_same<float, OUTPUT_T>::value) {
    assm.fmv_x_w(a0, fa0);
  } else if (std::is_same<double, OUTPUT_T>::value) {
    assm.fmv_x_d(a0, fa0);
  }
  assm.jr(ra);

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code =
      Factory::CodeBuilder(isolate, desc, CodeKind::FOR_TESTING).Build();

  using OINT_T = typename std::conditional<
      std::is_integral<OUTPUT_T>::value, OUTPUT_T,
      typename std::conditional<sizeof(OUTPUT_T) == 4, int32_t,
                                int64_t>::type>::type;
  using IINT_T = typename std::conditional<
      std::is_integral<INPUT_T>::value, INPUT_T,
      typename std::conditional<sizeof(INPUT_T) == 4, int32_t,
                                int64_t>::type>::type;
  auto f = GeneratedCode<OINT_T(IINT_T, IINT_T, IINT_T)>::FromCode(*code);

  auto res = f.Call(bit_cast<IINT_T>(input0), bit_cast<IINT_T>(input1),
                    bit_cast<IINT_T>(input2));
  return bit_cast<OUTPUT_T>(res);
}

template <typename T>
void GenAndRunTestForLoadStore(T value, Func test_generator) {
  DCHECK(sizeof(T) == 4 || sizeof(T) == 8);

  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, v8::internal::CodeObjectRequired::kYes);

  if (std::is_same<float, T>::value) {
    assm.fmv_w_x(fa0, a1);
  } else if (std::is_same<double, T>::value) {
    assm.fmv_d_x(fa0, a1);
  }

  test_generator(assm);

  if (std::is_same<float, T>::value) {
    assm.fmv_x_w(a0, fa0);
  } else if (std::is_same<double, T>::value) {
    assm.fmv_x_d(a0, fa0);
  }
  assm.jr(ra);

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code =
      Factory::CodeBuilder(isolate, desc, CodeKind::FOR_TESTING).Build();

  using INT_T = typename std::conditional<
      std::is_integral<T>::value, T,
      typename std::conditional<sizeof(T) == 4, int32_t, int64_t>::type>::type;

  auto f = GeneratedCode<INT_T(void* base, INT_T val)>::FromCode(*code);

  int64_t tmp = 0;
  auto res = f.Call(&tmp, bit_cast<INT_T>(value));
  CHECK_EQ(bit_cast<T>(res), value);
}

template <typename T, typename Func>
void GenAndRunTestForLRSC(T value, Func test_generator) {
  DCHECK(sizeof(T) == 4 || sizeof(T) == 8);

  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, v8::internal::CodeObjectRequired::kYes);

  if (std::is_same<float, T>::value) {
    assm.fmv_w_x(fa0, a1);
  } else if (std::is_same<double, T>::value) {
    assm.fmv_d_x(fa0, a1);
  }

  if (std::is_same<int32_t, T>::value) {
    assm.sw(a1, a0, 0);
  } else if (std::is_same<int64_t, T>::value) {
    UNREACHABLE();
  }
  test_generator(assm);

  if (std::is_same<float, T>::value) {
    assm.fmv_x_w(a0, fa0);
  } else if (std::is_same<double, T>::value) {
    assm.fmv_x_d(a0, fa0);
  }
  assm.jr(ra);

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code =
      Factory::CodeBuilder(isolate, desc, CodeKind::FOR_TESTING).Build();
#if defined(DEBUG)
  code->Print();
#endif
  using INT_T =
      typename std::conditional<sizeof(T) == 4, int32_t, int64_t>::type;

  T tmp = 0;
  auto f = GeneratedCode<INT_T(void* base, INT_T val)>::FromCode(*code);
  auto res = f.Call(&tmp, bit_cast<T>(value));
  CHECK_EQ(bit_cast<T>(res), static_cast<T>(0));
}

template <typename INPUT_T, typename OUTPUT_T, typename Func>
OUTPUT_T GenAndRunTestForAMO(INPUT_T input0, INPUT_T input1,
                             Func test_generator) {
  DCHECK(sizeof(INPUT_T) == 4 || sizeof(INPUT_T) == 8);
  DCHECK(sizeof(OUTPUT_T) == 4 || sizeof(OUTPUT_T) == 8);
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, v8::internal::CodeObjectRequired::kYes);

  // handle floating-point parameters
  if (std::is_same<float, INPUT_T>::value) {
    assm.fmv_w_x(fa0, a1);
    assm.fmv_w_x(fa1, a2);
  } else if (std::is_same<double, INPUT_T>::value) {
    assm.fmv_d_x(fa0, a1);
    assm.fmv_d_x(fa1, a2);
  }

  // store base integer
  if (std::is_same<int32_t, INPUT_T>::value ||
      std::is_same<uint32_t, INPUT_T>::value) {
    assm.sw(a1, a0, 0);
  } else if (std::is_same<int64_t, INPUT_T>::value ||
             std::is_same<uint64_t, INPUT_T>::value) {
    UNREACHABLE();
  }
  test_generator(assm);

  // handle floating-point result
  if (std::is_same<float, OUTPUT_T>::value) {
    assm.fmv_x_w(a0, fa0);
  } else if (std::is_same<double, OUTPUT_T>::value) {
    assm.fmv_x_d(a0, fa0);
  }

  // load written integer
  if (std::is_same<int32_t, INPUT_T>::value ||
      std::is_same<uint32_t, INPUT_T>::value) {
    assm.lw(a0, a0, 0);
  } else if (std::is_same<int64_t, INPUT_T>::value ||
             std::is_same<uint64_t, INPUT_T>::value) {
    UNREACHABLE();
  }

  assm.jr(ra);

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code =
      Factory::CodeBuilder(isolate, desc, CodeKind::FOR_TESTING).Build();
#if defined(DEBUG)
  code->Print();
#endif
  OUTPUT_T tmp = 0;
  auto f =
      GeneratedCode<OUTPUT_T(void* base, INPUT_T, INPUT_T)>::FromCode(*code);
  auto res = f.Call(&tmp, bit_cast<INPUT_T>(input0), bit_cast<INPUT_T>(input1));
  return bit_cast<OUTPUT_T>(res);
}

Handle<Code> AssembleCodeImpl(Func assemble);

template <typename Signature>
GeneratedCode<Signature> AssembleCode(Func assemble) {
  return GeneratedCode<Signature>::FromCode(*AssembleCodeImpl(assemble));
}

template <typename T>
T UseCanonicalNan(T x) {
  return isnan(x) ? std::numeric_limits<T>::quiet_NaN() : x;
}

}  // namespace internal
}  // namespace v8

#endif  // V8_CCTEST_TEST_HELPER_RISCV_H_
