#include "ricoh2a03.hpp"
#include <sstream>
#include <stdexcept>
#include <algorithm>

using namespace std;

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
        uint8_t instr = mem_read(pc);
        pc += 1;
        uint16_t pc_state = pc;
        const OpCode& opcode = *find_if(opcodes.begin(), opcodes.end(), [&instr] (const OpCode& oc) {return oc.code == instr;});

        switch(opcode.code) {
            case 0x00:
                return;
            case 0x85:
            case 0x95:
            case 0x8D:
            case 0x9D:
            case 0x99:
            case 0x81:
            case 0x91:
                sta(opcode.addr_mode);
                break;
            case 0xA9: // load accumulator
            case 0xA5: // load accumulator
            case 0xB5: // load accumulator
            case 0xAD: // load accumulator
            case 0xBD: // load accumulator
            case 0xB9: // load accumulator
            case 0xA1: // load accumulator
            case 0xB1: // load accumulator
                lda(opcode.addr_mode);
                break;
            case 0xAA: // transfer a(cc) to x
                tax();
                break;
            case 0xE8: // increment x
                inx();
                break;
            default:
                stringstream error;
                error << "Opcode: " << hex << opcode.code << dec << "is unsupported." << endl;
                throw runtime_error(error.str());

        }
        if(pc_state == pc) {
            pc += opcode.bytes - 1;
        }
    }
}

void Ricoh2a03::update_zero_and_negative_flags(uint8_t result) {
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
    uint16_t addr = get_operand_address(mode);
    uint8_t value = mem_read(addr);
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
    uint16_t addr = get_operand_address(mode);
    mem_write(addr, reg_acc);
}

// memory access

uint16_t Ricoh2a03::get_operand_address(AddressingMode mode) {
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
                uint8_t base = mem_read(pc);
                uint8_t ptr = base + reg_x;
                uint8_t lo = mem_read(ptr);
                uint8_t hi = mem_read((uint16_t) ptr + 1);
                return ((uint16_t) hi << 8) | lo;
            }
        case Indirect_Y:
            {
                uint8_t base = mem_read(pc);
                uint8_t lo = mem_read(base);
                uint8_t hi = mem_read((uint16_t) base + 1);
                uint16_t deref_base = ((uint16_t) hi << 8) | lo;
                return deref_base + reg_y;
            }
        case NoneAddressing:
        default:
            //std::cerr << std::vformat("Addressing mode: {:d} is unsupported.", std::make_format_args(mode)) << std::endl;
            stringstream error;
            error << "Addressing mode: " << int(mode) << "is unsupported." << endl;
            throw std::runtime_error(error.str());
    }
}

uint8_t Ricoh2a03::mem_read(uint16_t addr) {
    return memory[addr];
}

void Ricoh2a03::mem_write(uint16_t addr, uint8_t data) {
    memory[addr] = data;
}


uint16_t Ricoh2a03::mem_read_u16(uint16_t pos) {
    uint16_t lo = mem_read(pos);
    uint16_t hi = mem_read(pos + 1);
    return ((uint16_t) hi << 8) | lo;
}

void Ricoh2a03::mem_write_u16(uint16_t pos, uint16_t data) {
    uint16_t hi = (data >> 8);
    uint16_t lo = (data & 0xff);
    mem_write(pos, lo);
    mem_write(pos + 1, hi);
}

void Ricoh2a03::reset() {
    reg_acc = 0;
    reg_x = 0;
    reg_status = 0;

    pc = mem_read_u16(0xFFFC);
}

void Ricoh2a03::load_and_run(vector<uint8_t> program) {
    load(program);
    reset();
    run();
}

void Ricoh2a03::load(vector<std::uint8_t> program) {
    copy(program.begin(), program.end(), memory.begin() + 0x8000);
    mem_write_u16(0xFFFC, 0x8000);
}
