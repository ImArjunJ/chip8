#include <array>
#include <cstdint>
#include <expected>
#include <fstream>
#include <iostream>
#include <print>
#include <string_view>
#include <utility>

/*
cli tool to begin with?? chip8 -h
4096 Bytes of RAM from 0x000 to 0xFFF
0x000 to 0x1FF are reserved for the interpreter, should not be used by any programs
most chip8 start at 0x200, some begin at 0x600
0x200/0x600 to 0xFFF chip-8 Program / Data space (von-neumann)

16-key hexadecimal keypad layout:
1	2	3	C
4	5	6	D
7	8	9	E
A	0	B	F
*/

namespace chip8 {
  struct cpu_t {
    std::array<std::uint8_t, 4096> memory {};
    std::array<std::uint8_t, 16> registers {}; // VF register should not be used (flag used by some instructions)
    std::uint16_t i {}; // generally used to store memory addresses, rightmost 12 bits are usually used
    std::uint8_t delay_timer;
    std::uint8_t sound_timer;
    std::uint16_t pc;
    std::uint8_t sp;
    std::array<std::uint16_t, 16> stack {};
    std::array<std::array<std::uint8_t, 64>,32> display {}; // accessed display[y][x]
  };
  static cpu_t cpu;

  struct cartridge_t {
      std::array<std::uint16_t, 3584 / 2> instructions {};
      std::uint32_t size;
  };

  enum class load_error : std::uint8_t {
    FILE_ERROR,
  };

  // TODO: input sanitization, ensure valid ch8 format, ensure pos cannot overflow the arr etc. 
  std::expected<cartridge_t, load_error> load_cartridge(std::string_view file_path) {
    std::ifstream stream(file_path.data(), std::ios::binary | std::ios::ate);
    std::array<std::uint16_t, 3584/2> instructions {};
    if(!stream.is_open()) {
      return std::unexpected(load_error::FILE_ERROR);  
    }
    std::uint32_t pos = 0;
    const std::uint32_t file_size = stream.tellg(); 
    for (std::uint32_t i = 0; i < file_size; i += 2) {
      std::uint16_t instruction {};
      stream.seekg(i,std::ios::beg);
      stream.read(reinterpret_cast<char*>(&instruction), sizeof(instruction));
      instructions[pos] = instruction;
      pos++;
    }
    return cartridge_t{ .instructions=instructions, .size = pos };
  }

}

int main(int, char**){
  // TODO: cli setup? arguments?
  auto status = chip8::load_cartridge("./roms/ibm.ch8");
  if(!status) {
    std::println("load_cartridge error: {}", std::to_underlying(status.error()));
    return -1;
  }
  std::println("loaded {} instructions.", status.value().size);
  return 0;
}
