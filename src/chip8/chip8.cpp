#include "chip8.hpp"
#include <fstream>

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

// TODO: refactor instruction handling, need a system where we can recognize
// unrecognized instructions
bool emulator::step_instruction() {
  std::uint16_t instruction =
      (cpu.memory[cpu.pc] << 8) | cpu.memory[cpu.pc + 1];
  std::uint16_t msb = instruction & 0xF000;
  pc_modified = false;
  switch (instruction) {
  case 0x00E0: {
    cpu.display.fill(std::array<std::uint8_t, 64>{});
    break;
  }
  case 0x00EE: {
    cpu.pc = cpu.stack[cpu.sp];
    cpu.sp--;
    pc_modified = true;
    break;
  }
  }
  switch (msb) {
  case 0x1000: {
    cpu.pc = (instruction & 0x0FFF);
    pc_modified = true;
    break;
  }
  case 0x6000: {
    cpu.registers[(instruction & 0x0F00) >> 0x8] = instruction & 0x00FF;
    break;
  }
  case 0x7000: {
    cpu.registers[(instruction & 0x0F00) >> 0x8] += instruction & 0x00FF;
    break;
  }
  case 0xA000: {
    cpu.i = instruction & 0x0FFF;
    break;
  }
  case 0xD000: {
    uint8_t x_reg = (instruction & 0x0F00) >> 8;
    uint8_t y_reg = (instruction & 0x00F0) >> 4;
    uint8_t height = (instruction & 0x000F);

    uint8_t x_coord = cpu.registers[x_reg] % 64;
    uint8_t y_coord = cpu.registers[y_reg] % 32;

    cpu.registers[0xF] = 0;

    for (int row = 0; row < height; row++) {
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
  }
  if (!pc_modified) {
    cpu.pc += 2;
  }
  return false;
}

bool emulator::is_running() { return running; }
bool emulator::can_render() { return cpu.draw_flag; }

void emulator::render(window &window) {
  for (std::uint32_t y = 0; y < cpu.display.size(); ++y) {
    for (std::uint32_t x = 0; x < cpu.display[0].size(); ++x) {
      window.render(cpu.display);
    }
  }
}
} // namespace chip8