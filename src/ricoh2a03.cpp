#include "ricoh2a03.hpp"
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iostream>

using namespace std;

Ricoh2a03::Ricoh2a03() {
    pc = 0;
    reg_acc = 0;
    reg_x = 0;
    reg_y = 0;
    reg_status = 0;
    reg_sp = 0;
}

void Ricoh2a03::run() {
    run_with_callback([](Ricoh2a03&){});
}

void Ricoh2a03::run_with_callback(function<void(Ricoh2a03&)> f) {
    using enum AddressingMode;
    for(;;) {
        invoke(f, *this);
        uint8_t instr = mem_read(pc);
        pc += 1;
        uint16_t pc_state = pc;
        const OpCode& opcode = *find_if(opcodes.begin(), opcodes.end(), [&instr] (const OpCode& oc) {return oc.code == instr;});
	cout << "hej" << hex << unsigned(opcode.code) << dec << endl;
        switch(opcode.code) {
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
            case 0x00: // BRK
                return;
            case 0xD8: // CLD
                clear_status_flag(CpuStatus::DECIMAL_MODE);
                break;
            case 0x58: // CLI
                clear_status_flag(CpuStatus::INTERRUPT_DISABLE);
                break;
            case 0xB8: // CLV
                clear_status_flag(CpuStatus::OVERFLOW);
                break;
            case 0x18: // CLC
                clear_status_flag(CpuStatus::CARRY);
                break;
            case 0x38: // SEC
                set_status_flag(CpuStatus::CARRY);
                break;
            case 0x78: //SEI
                set_status_flag(CpuStatus::INTERRUPT_DISABLE);
                break;
            case 0xf8: // SED
                set_status_flag(CpuStatus::DECIMAL_MODE);
                break;
            case 0x48: // PHA
                stack_push(reg_acc);
                break;
            case 0x68:
                pla();
                break;
            case 0x08:
                php();
                break;
            case 0x28:
                plp();
                break;
            case 0x69:
            case 0x65:
            case 0x6D:
            case 0x7D:
            case 0x79:
            case 0x61:
            case 0x71:
                adc(opcode.addr_mode);
                break;
            case 0xE9:
            case 0xE5:
            case 0xF5:
            case 0xED:
            case 0xFD:
            case 0xF9:
            case 0xE1:
            case 0xF1:
                sbc(opcode.addr_mode);
                break;
            case 0x29:
            case 0x25:
            case 0x35:
            case 0x2D:
            case 0x3D:
            case 0x39:
            case 0x21:
            case 0x31:
                andop(opcode.addr_mode);
                break;
            case 0x49:
            case 0x45:
            case 0x55:
            case 0x4D:
            case 0x5D:
            case 0x59:
            case 0x41:
            case 0x51:
                eor(opcode.addr_mode);
                break;
            case 0x09:
            case 0x05:
            case 0x15:
            case 0x0D:
            case 0x1D:
            case 0x19:
            case 0x01:
            case 0x11:
                ora(opcode.addr_mode);
                break;
            case 0x4A:
                lsr_accumulator();
                break;
            case 0x46:
            case 0x56:
            case 0x4E:
            case 0x5E:
                lsr(opcode.addr_mode);
                break;
            case 0x0A:
                asl_accumulator();
                break;
            case 0x06:
            case 0x16:
            case 0x0E:
            case 0x1E:
		cout << "da" << endl;
                asl(opcode.addr_mode);
                break;
            case 0x2A:
                rol_accumulator();
                break;
            case 0x26:
            case 0x36:
            case 0x2E:
            case 0x3E:
                rol(opcode.addr_mode);
                break;
            case 0x6A:
                ror_accumulator();
                break;
            case 0x66:
            case 0x76:
            case 0x6E:
            case 0x7E:
                ror(opcode.addr_mode);
                break;
            case 0xE6:
            case 0xF6:
            case 0xEE:
            case 0xFE:
                inc(opcode.addr_mode);
                break;
            case 0xC8:
                iny();
                break;
            case 0xC6:
            case 0xD6:
            case 0xCE:
            case 0xDE:
                decop(opcode.addr_mode);
                break;
            case 0xCA:
                dex();
                break;
            case 0x88:
                dey();
                break;
            case 0xC9:
            case 0xC5:
            case 0xD5:
            case 0xCD:
            case 0xDD:
            case 0xD9:
            case 0xC1:
            case 0xD1:
                compare(opcode.addr_mode, reg_acc);
                break;
            case 0xC0:
            case 0xC4:
            case 0xCC:
                compare(opcode.addr_mode, reg_y);
                break;
            case 0xE0:
            case 0xE4:
            case 0xEC:
                compare(opcode.addr_mode, reg_x);
                break;
            case 0x4C: // JMP Absolute
                pc = mem_read_u16(pc);
                break;
            case 0x6c: // JMP Indirect
                {
                    uint16_t addr = mem_read_u16(pc);
                    // let indirect_ref = self.mem_read_u16(mem_address);
                    //6502 bug mode with with page boundary:
                    //  if address $3000 contains $40, $30FF contains $80, and $3100 contains $50,
                    // the result of JMP ($30FF) will be a transfer of control to $4080 rather than $5080 as you intended
                    // i.e. the 6502 took the low byte of the address from $30FF and the high byte from $3000

                    if((addr & 0x00FF) == 0x00FF) {
                        uint8_t lo = mem_read(addr);
                        uint8_t hi = mem_read(addr);
                        pc = (uint16_t)hi << 8 | lo;
                    } else {
                        pc = mem_read_u16(addr);
                    }
                    break;
                }
            case 0x20: // JSR
		{
		    cout << "da" << endl;
                    stack_push_u16(pc + 2 - 1);
		    uint16_t addr = mem_read_u16(pc);
                    pc = addr;
                    break;
		}
            case 0x60: // RTS
                pc = stack_pop_u16() + 1;
                break;
            case 0x40: // RTI
                reg_status = stack_pop();
                clear_status_flag(CpuStatus::BREAK);
                set_status_flag(CpuStatus::BREAK2);
                pc = stack_pop_u16();
                break;
            case 0xD0: // BNE
                branch(!(reg_status & CpuStatus::ZERO));
                break;
            case 0x70: // BVS
                branch(reg_status & CpuStatus::OVERFLOW);
                break;
            case 0x50: // BVC
                branch(!(reg_status & CpuStatus::OVERFLOW));
                break;
            case 0x10: // BPL
                branch(!(reg_status & CpuStatus::NEGATIVE));
                break;
            case 0x30: // BMI
                branch(reg_status & CpuStatus::NEGATIVE);
                break;
            case 0xF0: // BEQ
                branch(reg_status & CpuStatus::ZERO);
                break;
            case 0xB0: // BCS
                branch(reg_status & CpuStatus::CARRY);
                break;
            case 0x90: // BCC
                branch(!(reg_status & CpuStatus::CARRY));
                break;
            case 0x24:
            case 0x2C:
                bit(opcode.addr_mode);
                break;
            case 0x85:
            case 0x95:
            case 0x8D:
            case 0x9D:
            case 0x99:
            case 0x81:
            case 0x91:
                sta(opcode.addr_mode);
                break;
            case 0x86: // STX
            case 0x96:
            case 0x8E:
                {
                    uint16_t addr = get_operand_address(opcode.addr_mode);
                    mem_write(addr, reg_x);
                    break;
                }
            case 0x84: // STY
            case 0x94:
            case 0x8C:
                {
                    uint16_t addr = get_operand_address(opcode.addr_mode);
                    mem_write(addr, reg_y);
                    break;
                }
            case 0xA2:
            case 0xA6:
            case 0xAE:
            case 0xBE:
                ldx(opcode.addr_mode);
                break;
            case 0xA0:
            case 0xA4:
            case 0xB4:
            case 0xAC:
            case 0xBC:
                ldy(opcode.addr_mode);
                break;
            case 0xEA: // NOP
                break;
            case 0xA8: // TAY
                reg_y = reg_acc;
                update_zero_and_negative_flags(reg_y);
                break;
            case 0xBA: // TSX
                reg_x = reg_sp;
                update_zero_and_negative_flags(reg_x);
                break;
            case 0x8A: // TXA
                reg_acc = reg_x;
                update_zero_and_negative_flags(reg_acc);
                break;
            case 0x9A: // TXS
                reg_sp = reg_x;
                break;
            case 0x98: // TYA
                reg_acc = reg_y;
                update_zero_and_negative_flags(reg_acc);
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

void Ricoh2a03::ldy(AddressingMode mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t value = mem_read(addr);
    reg_y = value;
    update_zero_and_negative_flags(reg_y);
}

void Ricoh2a03::ldx(AddressingMode mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t value = mem_read(addr);
    reg_x = value;
    update_zero_and_negative_flags(reg_x);
}

void Ricoh2a03::lda(AddressingMode mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t value = mem_read(addr);
    reg_acc = value;
    update_zero_and_negative_flags(reg_acc);
}

void Ricoh2a03::sta(AddressingMode mode) {
    uint16_t addr = get_operand_address(mode);
    mem_write(addr, reg_acc);
}

void Ricoh2a03::andop(AddressingMode mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    set_reg_acc(data & reg_acc);
}

void Ricoh2a03::eor(AddressingMode mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    set_reg_acc(data ^ reg_acc);
}

void Ricoh2a03::ora(AddressingMode mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    set_reg_acc(data | reg_acc);
}

void Ricoh2a03::tax() {
    reg_x = reg_acc;
    update_zero_and_negative_flags(reg_x);
}

void Ricoh2a03::inx() {
    reg_x += 1;
    update_zero_and_negative_flags(reg_x);
}

void Ricoh2a03::iny() {
    reg_y += 1;
    update_zero_and_negative_flags(reg_y);
}

void Ricoh2a03::dey() {
    reg_y -= 1;
    update_zero_and_negative_flags(reg_y);
}

void Ricoh2a03::dex() {
    reg_x -= 1;
    update_zero_and_negative_flags(reg_x);
}

uint8_t Ricoh2a03::decop(AddressingMode mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    data -= 1;
    mem_write(addr, data);
    update_zero_and_negative_flags(data);
    return data;
}

void Ricoh2a03::pla() {
    uint8_t data = stack_pop();
    set_reg_acc(data);
}

void Ricoh2a03::plp() {
    reg_status = stack_pop();
    clear_status_flag(CpuStatus::BREAK);
    set_status_flag(CpuStatus::BREAK2);
}

void Ricoh2a03::php() {
    uint8_t flags = reg_status;
    flags |= CpuStatus::BREAK;
    flags |= CpuStatus::BREAK2;
    stack_push(flags);
}

void Ricoh2a03::bit(AddressingMode mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    uint8_t anded = reg_acc & data;

    if(anded == 0) {
        set_status_flag(CpuStatus::ZERO);
    } else {
        clear_status_flag(CpuStatus::ZERO);
    }

    if(data & 0b10000000) {
        set_status_flag(CpuStatus::NEGATIVE);
    }
    if(data & 0b01000000) {
        set_status_flag(CpuStatus::OVERFLOW);
    }
}

void Ricoh2a03::compare(AddressingMode mode,  uint8_t compare_with) {\
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);

    if(data <= compare_with) {
        set_status_flag(CpuStatus::CARRY);
    } else {
        clear_status_flag(CpuStatus::CARRY);
    }
    update_zero_and_negative_flags(compare_with - data);
}

void Ricoh2a03::branch(bool condition) {
    if(condition) {
        int8_t jump = mem_read(pc);
        uint16_t jump_addr = pc + 1 + jump;
        pc = jump_addr;
    }
}

void Ricoh2a03::sbc(AddressingMode mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    add_to_reg_acc(data ^ 0xFF);
}

void Ricoh2a03::adc(AddressingMode mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    add_to_reg_acc(data);
}

uint8_t Ricoh2a03::asl(AddressingMode mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);

    if((data >> 7) == 1) {
        set_status_flag(CpuStatus::CARRY);
    } else {
        clear_status_flag(CpuStatus::CARRY);
    }
    data = data << 1;
    mem_write(addr, data);

    update_zero_and_negative_flags(data);
    return data;
}

uint8_t Ricoh2a03::lsr(AddressingMode mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);

    if((data & 1) == 1) {
        set_status_flag(CpuStatus::CARRY);
    } else {
        clear_status_flag(CpuStatus::CARRY);
    }
    data = data >> 1;
    mem_write(addr, data);

    update_zero_and_negative_flags(data);
    return data;
}

uint8_t Ricoh2a03::rol(AddressingMode mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    bool old_carry = reg_status & CpuStatus::CARRY;

    if(data >> 7 == 1) {
        set_status_flag(CpuStatus::CARRY);
    } else {
        clear_status_flag(CpuStatus::CARRY);
    }
    data = data << 1;
    if(old_carry) {
        data = data | 1;
    }
    mem_write(addr, data);
    update_zero_and_negative_flags(data);
    return data;
}

uint8_t Ricoh2a03::ror(AddressingMode mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    bool old_carry = reg_status & CpuStatus::CARRY;

    if((data & 1) == 1) {
        set_status_flag(CpuStatus::CARRY);
    } else {
        clear_status_flag(CpuStatus::CARRY);
    }
    data = data >> 1;
    if(old_carry) {
        data = data | 0b10000000;
    }
    mem_write(addr, data);
    update_zero_and_negative_flags(data);
    return data;
}

uint8_t Ricoh2a03::inc(AddressingMode mode) {
    uint16_t addr = get_operand_address(mode);
    uint8_t data = mem_read(addr);
    data += 1;
    update_zero_and_negative_flags(data);
    return data;
}

// stack methods
uint8_t Ricoh2a03::stack_pop() {
    reg_sp += 1;
    return mem_read(STACK_ADDR + reg_sp);
}

void Ricoh2a03::stack_push(uint8_t data) {
    mem_write(STACK_ADDR + reg_sp, data);
}

void Ricoh2a03::stack_push_u16(uint16_t data) {
    uint8_t hi = data >> 8;
    uint8_t lo = data & 0xff;
    stack_push(hi);
    stack_push(lo);
}

uint16_t Ricoh2a03::stack_pop_u16() {
    uint16_t lo = stack_pop();
    uint16_t hi = stack_pop() << 8;
    return hi << 8 | lo;
}

// utility methods
void Ricoh2a03::set_status_flag(CpuStatus flag) {
    reg_status |= flag;
}

void Ricoh2a03::clear_status_flag(CpuStatus flag) {
    reg_status &= ~flag;
}

void Ricoh2a03::set_reg_acc(uint8_t value) {
    reg_acc = value;
    update_zero_and_negative_flags(reg_y);
}

void Ricoh2a03::add_to_reg_acc(uint8_t data) {
    uint16_t sum = reg_acc + data + (reg_status & CpuStatus::CARRY ? 1 : 0);
    bool carry = sum > 0xFF;

    if(carry) {
        set_status_flag(CpuStatus::CARRY);
    } else {
        clear_status_flag(CpuStatus::CARRY);
    }

    uint8_t result = sum;

    if(((data ^ result) & (result ^ reg_acc) & 0x80) != 0) {
        set_status_flag(CpuStatus::OVERFLOW);
    } else {
        clear_status_flag(CpuStatus::OVERFLOW);
    }

    set_reg_acc(result);
}

void Ricoh2a03::asl_accumulator() {
    uint8_t data = reg_acc;
    if(data >> 7 == 1) {
        set_status_flag(CpuStatus::CARRY);
    } else {
        clear_status_flag(CpuStatus::CARRY);
    }

    data = data << 1;
    set_reg_acc(data);
}

void Ricoh2a03::lsr_accumulator() {
    uint8_t data = reg_acc;
    if((data & 1) == 1) {
        set_status_flag(CpuStatus::CARRY);
    } else {
        clear_status_flag(CpuStatus::CARRY);
    }

    data = data >> 1;
    set_reg_acc(data);
}

void Ricoh2a03::rol_accumulator() {
    uint8_t data = reg_acc;
    const bool old_carry = reg_status & CpuStatus::CARRY;

    if((data >> 7) == 1) {
        set_status_flag(CpuStatus::CARRY);
    } else {
        clear_status_flag(CpuStatus::CARRY);
    }

    data = data << 1;
    if(old_carry) {
        data = data | 1;
    }
    set_reg_acc(data);
}

void Ricoh2a03::ror_accumulator() {
    uint8_t data = reg_acc;
    const bool old_carry = reg_status & CpuStatus::CARRY;

    if((data & 1) == 1) {
        set_status_flag(CpuStatus::CARRY);
    } else {
        clear_status_flag(CpuStatus::CARRY);
    }

    data = data >> 1;
    if(old_carry) {
        data = data | 0b10000000;
    }
    set_reg_acc(data);
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

// memory access
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
