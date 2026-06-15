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
  OR_VX_VY,
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
      this->type = instruction_type::CALL;
      this->modifies_pc = true;
      break;
    }
    case 0x3: {
      this->type = instruction_type::SE_VX_BYTE;
      break;
    }
    case 0x4: {
      this->type = instruction_type::SNE_VX_BYTE;
      break;
    }
    case 0x5: {
      this->type = instruction_type::SE_VX_VY;
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
      switch (n) {
      case 0x0: {
        this->type = instruction_type::LD_VX_VY;
        break;
      }
      case 0x1: {
        this->type = instruction_type::OR_VX_VY;
        break;
      }
      case 0x2: {
        this->type = instruction_type::AND_VX_VY;
        break;
      }
      case 0x3: {
        this->type = instruction_type::XOR_VX_VY;
        break;
      }
      case 0x4: {
        this->type = instruction_type::ADD_VX_VY;
        break;
      }
      case 0x5: {
        this->type = instruction_type::SUB_VX_VY;
        break;
      }
      case 0x6: {
        this->type = instruction_type::SHR_VX_VY;
        break;
      }
      case 0x7: {
        this->type = instruction_type::SUBN_VX_VY;
        break;
      }
      case 0xE: {
        this->type = instruction_type::SHL_VX_VY;
        break;
      }
      }
      break;
    }
    case 0x9: {
      this->type = instruction_type::SNE_VX_VY;
      break;
    }
    case 0xA: {
      this->type = instruction_type::LD_I_ADDR;
      break;
    }
    case 0xB: {
      this->type = instruction_type::JP_V0_ADDR;
      this->modifies_pc = true;
      break;
    }
    case 0xC: {
      this->type = instruction_type::RND_VX_BYTE;
      break;
    }
    case 0xD: {
      this->type = instruction_type::DRW_VX_VY;
      break;
    }
    case 0xE: {
      switch (kk) {
      case 0x9E: {
        this->type = instruction_type::SKP_VX;
        break;
      }
      case 0xA1: {
        this->type = instruction_type::SKNP_VX;
        break;
      }
      }
      break;
    }
    case 0xF: {
      switch (kk) {
      case 0x07: {
        this->type = instruction_type::LD_VX_DT;
        break;
      }
      case 0x0A: {
        this->type = instruction_type::LD_VX_K;
        break;
      }
      case 0x15: {
        this->type = instruction_type::LD_DT_VX;
        break;
      }
      case 0x65: {
        this->type = instruction_type::LD_VX_I;
        break;
      }
      case 0x18: {
        this->type = instruction_type::LD_ST_VX;
        break;
      }
      case 0x1E: {
        this->type = instruction_type::ADD_I_VX;
        break;
      }
      case 0x29: {
        this->type = instruction_type::LD_F_VX;
        break;
      }
      case 0x33: {
        this->type = instruction_type::LD_B_VX;
        break;
      }
      case 0x55: {
        this->type = instruction_type::LD_I_VX;
        break;
      }
      }
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