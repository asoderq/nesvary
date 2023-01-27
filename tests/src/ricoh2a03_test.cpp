#include "ricoh2a03.hpp"
#include "gtest/gtest.h"

class SuccessListener : public testing::EmptyTestEventListener {
  void OnTestPartResult(const testing::TestPartResult& result) override {
    if (result.type() == testing::TestPartResult::kSuccess) {
      printf("%s\n", result.message());
    }
  }
};

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    testing::UnitTest::GetInstance()->listeners().Append(new SuccessListener);
    return RUN_ALL_TESTS();
}

TEST(ricoh2a03, 0xa9_lda_immidiate_load_data)
{
    ricoh2a03 cpu = ricoh2a03();
    cpu.interpret({0xa9, 0x05, 0x00});
    ASSERT_EQ(cpu.reg_acc, 0x05);
    ASSERT_EQ(cpu.reg_status & 0b00000010, 0b00);
    ASSERT_EQ(cpu.reg_status & 0b10000000, 0);
}


TEST(ricoh2a03, 0xa9_lda_zero_flag)
{
    ricoh2a03 cpu = ricoh2a03();
    cpu.interpret({0xa9, 0x00, 0x00});
    ASSERT_EQ(cpu.reg_status & 0b00000010, 0b10);
}

TEST(ricoh2a03, 0xaa_tax_move_a_to_x)
{
    ricoh2a03 cpu = ricoh2a03();
    cpu.reg_acc = 10;
    cpu.interpret({0xaa, 0x00});
    ASSERT_EQ(cpu.reg_x, 10);
}
