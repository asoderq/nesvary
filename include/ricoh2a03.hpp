#pragma once

#include <cinttypes>
#include <vector>
#include <array>

class ricoh2a03 {
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
        ricoh2a03();
        // interpret program
        void run();
        void load_and_run(std::vector<std::uint8_t> program);
        void load(std::vector<std::uint8_t> program);

    private:
        // check and modify status, done at end of each instruction
        void update_zero_and_negative_flags(std::uint8_t result);

        // instructions
        void lda(std::uint8_t value);
        void tax();
        void inx();

        // memory access
        std::uint8_t mem_read(std::uint16_t addr);
        void mem_write(std::uint16_t addr, std::uint8_t data);
};
