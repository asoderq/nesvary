#pragma once

#include <cinttypes>
#include <vector>

class ricoh2a03 {

    protected:
        // register declaration
        std::uint16_t pc; // program counter
        std::uint8_t reg_acc; // accumulator
        std::uint8_t reg_x; // index x
        std::uint8_t reg_y; // index y
        std::uint8_t reg_status; // stack pointer
        std::uint8_t reg_p; // status
    public:
        // construct cpu
        ricoh2a03();
        // interpret program
        void interpret(std::vector<std::uint8_t> program);
};
