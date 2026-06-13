#pragma once
#include <cstdint>
namespace chip8 {
enum class instruction_type : std::uint16_t {
  NONE,
  CLS,
  RET,
  SYS,
  JP,
  CALL,
  SE_VX_BYTE,
  SNE_VX_BYTE,
  SE_VX_VY,
  LD_VX_BYTE,
  ADD_VX_BYTE,
  LD_VX_VY,
  AND_VX_VY,
  XOR_VX_VY,
  ADD_VX_VY,
  SUB_VX_VY,
  SHR_VX_VY,
  SUBN_VX_VY,
  SHL_VX_VY,
  SNE_VX_VY,
  LD_I_ADDR,
  JP_V0_ADDR,
  RND_VX_BYTE,
  DRW_VX_VY,
  SKP_VX,
  SKNP_VX,
  LD_VX_DT,
  LD_VX_K,
  LD_DT_VX,
  LD_ST_VX,
  ADD_I_VX,
  LD_F_VX,
  LD_B_VX,
  LD_I_VX,
  LD_VX_I
};

struct instruction_t {
  instruction_t() = default;
  instruction_t(std::uint16_t opcode) : instruction(opcode) {
    std::uint16_t type = (opcode & 0xF000) >> 12;
    this->x = (opcode & 0x0F00) >> 8;
    this->y = (opcode & 0x00F0) >> 4;
    this->nnn = (opcode & 0x0FFF);
    this->kk = (opcode & 0x00FF);
    this->n = (opcode & 0x000F);
    switch (type) {
    case 0x0: {
      switch (kk) {
      case 0xE0: {
        this->type = instruction_type::CLS;
        break;
      }
      case 0xEE: {
        this->type = instruction_type::RET;
        this->modifies_pc = true;
        break;
      }
      default:
        this->type = instruction_type::SYS;
      }
      break;
    }
    case 0x1: {
      this->type = instruction_type::JP;
      this->modifies_pc = true;
      break;
    }
    case 0x2: {
      break;
    }
    case 0x3: {
      break;
    }
    case 0x4: {
      break;
    }
    case 0x5: {
      break;
    }
    case 0x6: {
      this->type = instruction_type::LD_VX_BYTE;
      break;
    }
    case 0x7: {
      this->type = instruction_type::ADD_VX_BYTE;
      break;
    }
    case 0x8: {
      break;
    }
    case 0x9: {
      break;
    }
    case 0xA: {
      this->type = instruction_type::LD_I_ADDR;
      break;
    }
    case 0xB: {
      break;
    }
    case 0xC: {
      break;
    }
    case 0xD: {
      this->type = instruction_type::DRW_VX_VY;
      break;
    }
    case 0xE: {
      break;
    }
    case 0xF: {
      break;
    }
    }
  }
  operator instruction_type() const { return type; }
  std::uint16_t instruction;
  std::uint16_t x;
  std::uint16_t y;
  std::uint16_t n;
  std::uint16_t kk;
  std::uint16_t nnn;
  instruction_type type{instruction_type::NONE};
  bool modifies_pc = false;
};
} // namespace chip8