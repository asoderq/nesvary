#include "ricoh2a03.hpp"

ricoh2a03::ricoh2a03() {
    pc = 0;
    reg_acc = 0;
    reg_x = 0;
    reg_y = 0;
    reg_status = 0;
    reg_p = 0;
}

void ricoh2a03::interpret(std::vector<std::uint8_t> program) {
    pc = 0;

    for(;;) {
        std::uint8_t opcode = program[pc];
        pc += 1;

        switch(opcode) {
            case 0x00:
                return;
            case 0xA9:
                {
                std::uint8_t param = program[pc];
                pc += 1;
                lda(param);
                break;
                }
            case 0xAA:
                tax();
                break;
            case 0xE8:
                inx();

        }
    }
}

void ricoh2a03::update_zero_and_negative_flags(std::uint8_t result) {
    if(result == 0) {
        reg_status = reg_status | 0b00000010;
    } else {
        reg_status = reg_status & 0b11111101;
    }

    if((result & 0b10000000) != 0) {
        reg_status = reg_status | 0b10000000;
    } else {
        reg_status = reg_status & 0b01111111;
    }
}

void ricoh2a03::lda(std::uint8_t value) {
    reg_acc = value;
    update_zero_and_negative_flags(reg_acc);
}

void ricoh2a03::tax() {
    reg_x = reg_acc;
    update_zero_and_negative_flags(reg_x);
}

void ricoh2a03::inx() {
    reg_x += 1;
    update_zero_and_negative_flags(reg_x);
}
