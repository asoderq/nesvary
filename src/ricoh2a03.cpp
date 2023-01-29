#include "ricoh2a03.hpp"
#include <sstream>
#include <stdexcept>

Ricoh2a03::Ricoh2a03() {
    pc = 0;
    reg_acc = 0;
    reg_x = 0;
    reg_y = 0;
    reg_status = 0;
    reg_p = 0;
}

void Ricoh2a03::run() {
    using enum AddressingMode;
    for(;;) {
        std::uint8_t opcode = mem_read(pc);
        pc += 1;

        switch(opcode) {
            case 0x00:
                return;
            case 0x85:
                sta(ZeroPage);
                pc += 1;
                return;
            case 0x95:
                sta(ZeroPage_X);
                pc += 1;
                return;
            case 0xA9: // load accumulator
                lda(Immediate);
                pc += 1;
                break;
            case 0xA5: // load accumulator
                lda(ZeroPage);
                pc += 1;
                break;
            case 0xAD: // load accumulator
                lda(Absolute);
                pc += 2;
                break;
            case 0xAA: // transfer a(cc) to x
                tax();
                break;
            case 0xE8: // increment x
                inx();
                break;
            default:
                std::stringstream error;
                error << "Opcode: " << std::hex << opcode << std::dec << "is unsupported." << std::endl;
                throw std::runtime_error(error.str());

        }
    }
}

void Ricoh2a03::update_zero_and_negative_flags(std::uint8_t result) {
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

void Ricoh2a03::lda(AddressingMode mode) {
    std::uint16_t addr = get_operand_address(mode);
    std::uint8_t value = mem_read(addr);
    reg_acc = value;
    update_zero_and_negative_flags(reg_acc);
}

void Ricoh2a03::tax() {
    reg_x = reg_acc;
    update_zero_and_negative_flags(reg_x);
}

void Ricoh2a03::inx() {
    reg_x += 1;
    update_zero_and_negative_flags(reg_x);
}

void Ricoh2a03::sta(AddressingMode mode) {
    std::uint16_t addr = get_operand_address(mode);
    mem_write(addr, reg_acc);
}

// memory access

std::uint16_t Ricoh2a03::get_operand_address(AddressingMode mode) {
    using enum AddressingMode;
    switch(mode) {
        case Immediate:
            return pc;
        case ZeroPage:
            return mem_read(pc);
        case Absolute:
            return mem_read_u16(pc);
        case ZeroPage_X:
            return mem_read(pc) + reg_x;
        case ZeroPage_Y:
            return mem_read(pc) + reg_y;
        case Absolute_X:
            return mem_read_u16(pc) + reg_x;
        case Absolute_Y:
            return mem_read_u16(pc) + reg_y;
        case Indirect_X:
            {
                std::uint8_t base = mem_read(pc);
                std::uint8_t ptr = base + reg_x;
                std::uint8_t lo = mem_read(ptr);
                std::uint8_t hi = mem_read((std::uint16_t) ptr + 1);
                return ((std::uint16_t) hi << 8) | lo;
            }
        case Indirect_Y:
            {
                std::uint8_t base = mem_read(pc);
                std::uint8_t lo = mem_read(base);
                std::uint8_t hi = mem_read((std::uint16_t) base + 1);
                std::uint16_t deref_base = ((std::uint16_t) hi << 8) | lo;
                return deref_base + reg_y;
            }
        case NoneAddressing:
        default:
            //std::cerr << std::vformat("Addressing mode: {:d} is unsupported.", std::make_format_args(mode)) << std::endl;
            std::stringstream error;
            error << "Addressing mode: " << int(mode) << "is unsupported." << std::endl;
            throw std::runtime_error(error.str());
    }
}

std::uint8_t Ricoh2a03::mem_read(std::uint16_t addr) {
    return memory[addr];
}

void Ricoh2a03::mem_write(std::uint16_t addr, std::uint8_t data) {
    memory[addr] = data;
}


std::uint16_t Ricoh2a03::mem_read_u16(std::uint16_t pos) {
    std::uint16_t lo = mem_read(pos);
    std::uint16_t hi = mem_read(pos + 1);
    return ((std::uint16_t) hi << 8) | lo;
}

void Ricoh2a03::mem_write_u16(std::uint16_t pos, std::uint16_t data) {
    std::uint16_t hi = (data >> 8);
    std::uint16_t lo = (data & 0xff);
    mem_write(pos, lo);
    mem_write(pos + 1, hi);
}

void Ricoh2a03::reset() {
    reg_acc = 0;
    reg_x = 0;
    reg_status = 0;

    pc = mem_read_u16(0xFFFC);
}

void Ricoh2a03::load_and_run(std::vector<std::uint8_t> program) {
    load(program);
    reset();
    run();
}

void Ricoh2a03::load(std::vector<std::uint8_t> program) {
    std::copy(program.begin(), program.end(), memory.begin() + 0x8000);
    mem_write_u16(0xFFFC, 0x8000);
}
