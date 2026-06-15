#include "chip8.hpp"
#include "instruction.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <print>
#include <random>
#include <string>
#include <utility>

namespace chip8 {
static std::random_device rd;
static std::mt19937 gen(rd());

std::expected<void, load_error>
emulator::load_cartridge(std::string_view file_path) {
  constexpr std::size_t rom_start = 0x200;

  if (cpu.memory.size() <= rom_start) {
    return std::unexpected(load_error::FILE_ERROR);
  }

  std::ifstream stream(std::string(file_path),
                       std::ios::binary | std::ios::ate);
  if (!stream.is_open()) {
    return std::unexpected(load_error::FILE_ERROR);
  }

  const std::streampos end_pos = stream.tellg();
  if (end_pos == std::streampos(-1)) {
    return std::unexpected(load_error::FILE_ERROR);
  }

  const auto file_size = static_cast<std::size_t>(end_pos);
  const auto max_rom_size = cpu.memory.size() - rom_start;

  if (file_size > max_rom_size) {
    return std::unexpected(load_error::ROM_TOO_LARGE);
  }

  stream.seekg(0, std::ios::beg);
  if (!stream.good()) {
    return std::unexpected(load_error::FILE_ERROR);
  }

  stream.read(reinterpret_cast<char *>(cpu.memory.data() + rom_start),
              static_cast<std::streamsize>(file_size));

  if (stream.gcount() != static_cast<std::streamsize>(file_size)) {
    return std::unexpected(load_error::FILE_ERROR);
  }

  cpu.pc = rom_start;
  running = true;

  return {};
}

bool emulator::step_instruction() {
  if (cpu.pc + 1 >= cpu.memory.size()) {
    std::println(std::cerr, "program counter out of bounds: {:#06x}", cpu.pc);
    return false;
  }

  const auto raw_instruction = static_cast<std::uint16_t>(
      (static_cast<std::uint16_t>(cpu.memory[cpu.pc]) << 8) |
      static_cast<std::uint16_t>(cpu.memory[cpu.pc + 1]));

  instruction_t instruction = raw_instruction;
  bool pc_modified = false;

  if (instruction.x >= cpu.registers.size() ||
      instruction.y >= cpu.registers.size()) {
    std::println(std::cerr,
                 "instruction x or y out of bounds: {:#06x}, {:#06x}",
                 instruction.x, instruction.y);
    return false;
  }

  switch (instruction) {
  case instruction_type::SYS: {
    break;
  }

  case instruction_type::CLS: {
    cpu.display.fill(std::array<std::uint8_t, 64>{});
    cpu.draw_flag = true;
    break;
  }

  case instruction_type::RET: {
    if (cpu.sp == 0) {
      std::println(std::cerr, "stack underflow on RET");
      return false;
    }

    cpu.pc = cpu.stack[--cpu.sp];
    pc_modified = true;
    break;
  }

  case instruction_type::JP: {
    cpu.pc = instruction.nnn;
    pc_modified = true;
    break;
  }

  case instruction_type::CALL: {
    if (cpu.sp >= cpu.stack.size()) {
      std::println(std::cerr, "stack overflow on CALL");
      return false;
    }

    cpu.stack[cpu.sp++] = cpu.pc + 2;
    cpu.pc = instruction.nnn;
    pc_modified = true;
    break;
  }

  case instruction_type::SE_VX_BYTE: {
    if (cpu.registers[instruction.x] == instruction.kk) {
      cpu.pc += 2;
    }
    break;
  }

  case instruction_type::SNE_VX_BYTE: {
    if (cpu.registers[instruction.x] != instruction.kk) {
      cpu.pc += 2;
    }
    break;
  }

  case instruction_type::SE_VX_VY: {
    if (cpu.registers[instruction.x] == cpu.registers[instruction.y]) {
      cpu.pc += 2;
    }
    break;
  }

  case instruction_type::RND_VX_BYTE: {
    std::uniform_int_distribution<uint16_t> distrib(0, 255);
    std::uint8_t rand_byte = static_cast<uint8_t>(distrib(gen));
    cpu.registers[instruction.x] = rand_byte & instruction.kk;
    break;
  }

  case instruction_type::SNE_VX_VY: {
    if (cpu.registers[instruction.x] != cpu.registers[instruction.y]) {
      cpu.pc += 2;
    }
    break;
  }

  case instruction_type::AND_VX_VY: {
    cpu.registers[instruction.x] =
        cpu.registers[instruction.x] & cpu.registers[instruction.y];
    break;
  }

  case instruction_type::ADD_VX_VY: {
    uint16_t sum = cpu.registers[instruction.x] + cpu.registers[instruction.y];

    cpu.registers[instruction.x] = sum & 0xFF;
    if (sum > 255) {
      cpu.registers[0xF] = 1;
    } else {
      cpu.registers[0xF] = 0;
    }
    break;
  }

  case instruction_type::SUB_VX_VY: {
    std::uint8_t flag =
        (cpu.registers[instruction.x] >= cpu.registers[instruction.y]) ? 1 : 0;

    cpu.registers[instruction.x] =
        cpu.registers[instruction.x] - cpu.registers[instruction.y];
    cpu.registers[0xF] = flag;
    break;
  }

  case instruction_type::SUBN_VX_VY: {
    std::uint8_t flag =
        (cpu.registers[instruction.y] >= cpu.registers[instruction.x]) ? 1 : 0;

    cpu.registers[instruction.x] =
        cpu.registers[instruction.y] - cpu.registers[instruction.x];
    cpu.registers[0xF] = flag;
    break;
  }

  case instruction_type::SHR_VX_VY: {
    std::uint8_t source_value = cpu.registers[instruction.x];
    std::uint8_t lsb = source_value & 0x01;

    cpu.registers[instruction.x] = source_value >> 1;
    cpu.registers[0xF] = lsb;
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

  case instruction_type::LD_VX_VY: {
    cpu.registers[instruction.x] = cpu.registers[instruction.y];
    break;
  }

  case instruction_type::OR_VX_VY: {
    cpu.registers[instruction.x] |= cpu.registers[instruction.y];
    break;
  }

  case instruction_type::XOR_VX_VY: {
    cpu.registers[instruction.x] ^= cpu.registers[instruction.y];
    break;
  }

  case instruction_type::LD_I_ADDR: {
    cpu.i = instruction.nnn;
    break;
  }

  case instruction_type::ADD_I_VX: {
    cpu.i += cpu.registers[instruction.x];
    break;
  }
  case instruction_type::SHL_VX_VY: {
    std::uint8_t flag = (cpu.registers[instruction.x] >> 7) & 1;

    cpu.registers[instruction.x] = (cpu.registers[instruction.x] << 1) & 0xFF;
    cpu.registers[0xF] = flag;
    break;
  }
  case instruction_type::LD_B_VX: {
    if (cpu.i + 2 >= cpu.memory.size()) {
      std::println(std::cerr, "LD_B_VX, cpu.I out of bounds: {:#06x}", cpu.i);
      return false;
    }
    cpu.memory[cpu.i] = cpu.registers[instruction.x] / 100;
    cpu.memory[cpu.i + 1] = (cpu.registers[instruction.x] / 10) % 10;
    cpu.memory[cpu.i + 2] = cpu.registers[instruction.x] % 10;
    break;
  }
  case instruction_type::JP_V0_ADDR: {
    cpu.pc = instruction.nnn + cpu.registers[0];
    break;
  }
  case instruction_type::LD_F_VX: {
    cpu.i =
        cpu.FONT_START_ADDRESS + ((cpu.registers[instruction.x] & 0x0F) * 5);
    break;
  }
  case instruction_type::DRW_VX_VY: {
    const auto x_coord =
        static_cast<std::uint8_t>(cpu.registers[instruction.x] % 64);
    const auto y_coord =
        static_cast<std::uint8_t>(cpu.registers[instruction.y] % 32);

    cpu.registers[0xF] = 0;

    for (std::uint8_t row = 0; row < instruction.n; ++row) {
      const auto sprite_addr =
          static_cast<std::size_t>(cpu.i) + static_cast<std::size_t>(row);

      if (sprite_addr >= cpu.memory.size()) {
        std::println(std::cerr, "sprite read out of bounds at I={:#06x}",
                     cpu.i);
        return false;
      }

      const std::uint8_t sprite_byte = cpu.memory[sprite_addr];

      for (std::uint8_t col = 0; col < 8; ++col) {
        const std::uint8_t sprite_pixel = sprite_byte & (0x80 >> col);

        if (sprite_pixel == 0) {
          continue;
        }

        const int target_x = x_coord + col;
        const int target_y = y_coord + row;

        if (target_x >= 64 || target_y >= 32) {
          continue;
        }

        if (cpu.display[target_y][target_x] == 1) {
          cpu.registers[0xF] = 1;
        }

        cpu.display[target_y][target_x] ^= 1;
      }
    }

    cpu.draw_flag = true;
    break;
  }

  case instruction_type::SKP_VX: {
    const auto key = static_cast<std::size_t>(cpu.registers[instruction.x]);
    const bool pressed = key < cpu.keys.size() && cpu.keys[key];

    if (pressed) {
      cpu.pc += 2;
    }

    break;
  }

  case instruction_type::SKNP_VX: {
    const auto key = static_cast<std::size_t>(cpu.registers[instruction.x]);
    const bool pressed = key < cpu.keys.size() && cpu.keys[key];

    if (!pressed) {
      cpu.pc += 2;
    }

    break;
  }

  case instruction_type::LD_VX_DT: {
    cpu.registers[instruction.x] = cpu.delay_timer;
    break;
  }

  case instruction_type::LD_DT_VX: {
    cpu.delay_timer = cpu.registers[instruction.x];
    break;
  }

  case instruction_type::LD_VX_K: {
    bool pressed = false;
    std::uint8_t first_key = 0;

    for (std::size_t i = 0; i < cpu.keys.size(); ++i) {
      if (cpu.keys[i]) {
        pressed = true;
        first_key = static_cast<std::uint8_t>(i);
        break;
      }
    }

    if (!pressed) {
      pc_modified = true;
      break;
    }

    cpu.registers[instruction.x] = first_key;
    break;
  }

  case instruction_type::LD_ST_VX: {
    cpu.sound_timer = cpu.registers[instruction.x];
    break;
  }

  case instruction_type::LD_I_VX: {
    const auto start = static_cast<std::size_t>(cpu.i);
    const auto end = start + static_cast<std::size_t>(instruction.x);

    if (end >= cpu.memory.size()) {
      std::println(std::cerr, "memory write out of bounds in Fx55 at I={:#06x}",
                   cpu.i);
      return false;
    }

    for (std::uint8_t r = 0; r <= instruction.x; ++r) {
      cpu.memory[start + r] = cpu.registers[r];
    }

    break;
  }

  case instruction_type::LD_VX_I: {
    const auto start = static_cast<std::size_t>(cpu.i);
    const auto end = start + static_cast<std::size_t>(instruction.x);

    if (end >= cpu.memory.size()) {
      std::println(std::cerr, "memory read out of bounds in Fx65 at I={:#06x}",
                   cpu.i);
      return false;
    }

    for (std::uint8_t r = 0; r <= instruction.x; ++r) {
      cpu.registers[r] = cpu.memory[start + r];
    }

    break;
  }

  default: {
    std::println(std::cerr, "unrecognized instruction: {:#06x}",
                 instruction.instruction);
    return false;
  }
  }

  if (!pc_modified) {
    cpu.pc += 2;
  }

  return true;
}

bool emulator::is_running() { return running; }

bool emulator::can_render() { return cpu.draw_flag; }

void emulator::stop() { running = false; }

std::uint8_t emulator::get_sound_timer() { return cpu.sound_timer; }

void emulator::decrement_timers() {
  if (cpu.delay_timer > 0) {
    cpu.delay_timer--;
  }
  if (cpu.sound_timer > 0) {
    cpu.sound_timer--;
  }
}

void emulator::set_key(key_t key, bool state) {
  const auto index = static_cast<std::size_t>(std::to_underlying(key.code));

  if (index < cpu.keys.size()) {
    cpu.keys[index] = state;
  }
}

void emulator::render(window &window) {
  window.render(cpu.display);
  cpu.draw_flag = false;
}

} // namespace chip8