#include "ricoh2a03.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

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

TEST(Ricoh2a03, 0xa9_lda_immidiate_load_data)
{
    Ricoh2a03 cpu = Ricoh2a03();
    cpu.load_and_run({0xa9, 0x05, 0x00});
    ASSERT_EQ(cpu.reg_acc, 0x05);
    ASSERT_EQ(cpu.reg_status & 0b00000010, 0b00);
    ASSERT_EQ(cpu.reg_status & 0b10000000, 0);
}


TEST(Ricoh2a03, 0xa9_lda_zero_flag)
{
    Ricoh2a03 cpu = Ricoh2a03();
    cpu.load_and_run({0xa9, 0x00, 0x00});
    ASSERT_EQ(cpu.reg_status & 0b00000010, 0b10);
}

TEST(Ricoh2a03, 0xaa_tax_move_a_to_x)
{
    Ricoh2a03 cpu = Ricoh2a03();
    cpu.load({0xaa, 0x00});
    cpu.reset();
    cpu.reg_acc = 10;
    cpu.run();
    ASSERT_EQ(cpu.reg_x, 10);
}


TEST(Ricoh2a03, 0xe8_inx_overflow) {
    Ricoh2a03 cpu = Ricoh2a03();
    cpu.load({0xe8, 0xe8, 0x00});
    cpu.reset();
    cpu.reg_x = 0xff;
    cpu.run();
    ASSERT_EQ(cpu.reg_x, 1);
}


TEST(Ricoh2a03, 0xe8_5ops_together) {
    Ricoh2a03 cpu = Ricoh2a03();
    cpu.load_and_run({0xa9, 0xc0, 0xaa, 0xe8, 0x00});
    ASSERT_EQ(cpu.reg_x, 0xc1);
}

TEST(Ricoh2a03, load) {
    Ricoh2a03 cpu = Ricoh2a03();
    std::vector<std::uint8_t> program = {0xa9, 0x05, 0x00};
    cpu.load(program);
    ASSERT_THAT(std::vector<uint8_t>({cpu.memory.begin() +
            0x8000, cpu.memory.begin() + 0x8003}),
            testing::ElementsAreArray(program));
}

TEST(Ricoh2a03, lda_from_memory) {
    Ricoh2a03 cpu = Ricoh2a03();
    cpu.mem_write(0x10, 0x55);
    cpu.load_and_run({0xa5, 0x10, 0x00});
    ASSERT_EQ(cpu.reg_acc, 0x55);
}
