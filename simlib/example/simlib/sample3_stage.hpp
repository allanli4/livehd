#pragma once
#include <time.h>
struct Sample3_stage {
  uint64_t hidx;
 // vcd::VCDWriter vcd_writer{my_vcd}
  UInt<32> to1_b;

  std::array<UInt<32>, 256> memory;

  uint8_t reset_iterator;
  UInt<32> tmp;
  UInt<32> tmp2;

    Sample3_stage(uint64_t _hidx);
  //  Sample3_stage(std::string my_vcd);
  void reset_cycle();

  #ifdef SIMLIB_VCD
 //   vcd::VarPtr vcd_to1_b = vcd_writer.register_var("s3", "to1_b[31:0]", vcd::VariableType::wire, 32);

  void vcd_cycle(UInt<1> s1_to3_cValid, UInt<32> s1_to3_c, UInt<1> s2_to3_dValid, UInt<32> s2_to3_d);
#else
  void cycle(UInt<1> s1_to3_cValid, UInt<32> s1_to3_c, UInt<1> s2_to3_dValid, UInt<32> s2_to3_d);
#endif

#ifdef SIMLIB_TRACE
  void add_signature(Simlib_signature &sign);
#endif
};

