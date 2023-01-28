#include "ricoh2a03.hpp"

ricoh2a03::ricoh2a03() {
    pc = 0;
    reg_acc = 0;
    reg_x = 0;
    reg_y = 0;
    reg_status = 0;
    reg_p = 0;
}

void ricoh2a03::run() {
    for(;;) {
        std::uint8_t opcode = mem_read(pc);
        pc += 1;

        switch(opcode) {
            case 0x00:
                return;
            case 0xA9:
                {
                std::uint8_t param = mem_read(pc);
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

// instruction

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

// memory access

std::uint8_t ricoh2a03::mem_read(std::uint16_t addr) {
    return memory[addr];
}

void ricoh2a03::mem_write(std::uint16_t addr, std::uint8_t data) {
    memory[addr] = data;
}


std::uint16_t ricoh2a03::mem_read_u16(std::uint16_t pos) {
    std::uint16_t lo = mem_read(pos);
    std::uint16_t hi = mem_read(pos + 1);
    return (hi << 8) | lo;
}

void ricoh2a03::mem_write_u16(std::uint16_t pos, std::uint16_t data) {
    std::uint16_t hi = (data >> 8);
    std::uint16_t lo = (data & 0xff);
    mem_write(pos, lo);
    mem_write(pos + 1, hi);
}

void ricoh2a03::reset() {
    reg_acc = 0;
    reg_x = 0;
    reg_status = 0;

    pc = mem_read_u16(0xFFFC);
}

void ricoh2a03::load_and_run(std::vector<std::uint8_t> program) {
    load(program);
    reset();
    run();
}

void ricoh2a03::load(std::vector<std::uint8_t> program) {
    std::copy(program.begin(), program.end(), memory.begin());
    mem_write_u16(0xFFFC, 0x8000);
}
