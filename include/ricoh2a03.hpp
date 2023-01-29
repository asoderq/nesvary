#pragma once

#include <cinttypes>
#include <vector>
#include <array>

enum AddressingMode {
   Immediate,
   ZeroPage,
   ZeroPage_X,
   ZeroPage_Y,
   Absolute,
   Absolute_X,
   Absolute_Y,
   Indirect_X,
   Indirect_Y,
   NoneAddressing,
};

class Ricoh2a03 {
    public:
        // register declaration
        std::uint16_t pc; // program counter
        std::uint8_t reg_acc; // accumulator
        std::uint8_t reg_x; // index x
        std::uint8_t reg_y; // index y
        std::uint8_t reg_status; // stack pointer
        std::uint8_t reg_p; // status

        // memory
        std::array<std::uint8_t, 0xFFFF> memory;

        // construct cpu
        Ricoh2a03();
        // interpret program
        void run();
        void reset();
        void load_and_run(std::vector<std::uint8_t> program);
        void load(std::vector<std::uint8_t> program);

        // check and modify status, done at end of each instruction
        void update_zero_and_negative_flags(std::uint8_t result);

        // instructions
        void lda(AddressingMode mode);
        void tax();
        void inx();
        void sta(AddressingMode mode);

        // memory access
        std::uint16_t get_operand_address(AddressingMode mode);
        std::uint8_t mem_read(std::uint16_t addr);
        void mem_write(std::uint16_t addr, std::uint8_t data);
        std::uint16_t mem_read_u16(std::uint16_t pos);
        void mem_write_u16(std::uint16_t pos, std::uint16_t data);
};
