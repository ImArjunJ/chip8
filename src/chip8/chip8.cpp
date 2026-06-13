#include "chip8.hpp"
#include "instruction.hpp"
#include <fstream>
#include <iostream>
#include <print>

namespace chip8 {
std::expected<void, load_error>
emulator::load_cartridge(std::string_view file_path) {
  std::ifstream stream(file_path.data(), std::ios::binary | std::ios::ate);
  if (!stream.is_open()) {
    return std::unexpected(load_error::FILE_ERROR);
  }
  const std::uint32_t file_size = stream.tellg();

  if (file_size > (4096 - 0x200)) {
    return std::unexpected(load_error::ROM_TOO_LARGE);
  }

  stream.seekg(0, std::ios::beg);
  stream.read(reinterpret_cast<char *>(&cpu.memory[0x200]), file_size);

  return {};
}

bool emulator::step_instruction() {
  instruction_t instruction =
      (cpu.memory[cpu.pc] << 8) | cpu.memory[cpu.pc + 1];
  switch (instruction) {
  case instruction_type::CLS: {
    cpu.display.fill(std::array<std::uint8_t, 64>{});
    break;
  }
  case instruction_type::RET: {
    cpu.pc = cpu.stack[cpu.sp];
    cpu.sp--;
    pc_modified = true;
    break;
  }
  case instruction_type::JP: {
    cpu.pc = instruction.nnn;
    pc_modified = true;
    break;
  }
  case instruction_type::LD_VX_BYTE: {
    cpu.registers[instruction.x] = instruction.kk;
    break;
  }
  case instruction_type::ADD_VX_BYTE: {
    cpu.registers[instruction.x] += instruction.kk;
    break;
  }
  case instruction_type::LD_I_ADDR: {
    cpu.i = instruction.nnn;
    break;
  }
  case instruction_type::DRW_VX_VY: {

    uint8_t x_coord = cpu.registers[instruction.x] % 64;
    uint8_t y_coord = cpu.registers[instruction.y] % 32;

    cpu.registers[0xF] = 0;

    for (int row = 0; row < instruction.n; row++) {
      uint8_t sprite_byte = cpu.memory[cpu.i + row];

      for (int col = 0; col < 8; col++) {
        uint8_t sprite_pixel = (sprite_byte & (0x80 >> col));

        if (sprite_pixel != 0) {
          int target_x = x_coord + col;
          int target_y = y_coord + row;

          if (target_x < 64 && target_y < 32) {
            if (cpu.display[target_y][target_x] == 1) {
              cpu.registers[0xF] = 1;
            }
            cpu.display[target_y][target_x] ^= 1;
          }
        }
      }
    }
    cpu.draw_flag = true;
    break;
  }
  default: {
    std::println(std::cerr, "unrecognized instruction: {:#06x}",
                 instruction.instruction);
    return false;
  }
  }
  if (!instruction.modifies_pc) {
    cpu.pc += 2;
  }
  return true;
}

bool emulator::is_running() { return running; }
bool emulator::can_render() { return cpu.draw_flag; }
void emulator::stop() { running = false; }

void emulator::render(window &window) {
  for (std::uint32_t y = 0; y < cpu.display.size(); ++y) {
    for (std::uint32_t x = 0; x < cpu.display[0].size(); ++x) {
      window.render(cpu.display);
    }
  }
}
} // namespace chip8