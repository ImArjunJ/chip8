#pragma once
#include "window.hpp"
#include <array>
#include <cstdint>
#include <expected>
#include <string_view>
#include <thread>

namespace chip8 {
enum class load_error : std::uint8_t { FILE_ERROR, ROM_TOO_LARGE };

struct cpu_t {
  std::array<std::uint8_t, 4096> memory{};
  std::array<std::uint8_t, 16> registers{}; // VF register should not be used
                                            // (flag used by some instructions)
  std::uint16_t i{}; // generally used to store memory addresses, rightmost 12
                     // bits are usually used
  std::uint8_t delay_timer{};
  std::uint8_t sound_timer{};
  std::uint16_t pc{0x200};
  std::uint8_t sp{};
  bool draw_flag{};
  std::array<std::uint16_t, 16> stack{};
  std::array<std::array<std::uint8_t, 64>, 32>
      display{}; // accessed display[y][x]
};

class emulator {
public:
  emulator() = default;
  std::expected<void, load_error> load_cartridge(std::string_view file_path);
  bool step_instruction();
  void render(chip8::window &window);
  bool is_running();
  bool can_render();
  void stop();

private:
  chip8::cpu_t cpu{};
  bool running = true;
  bool pc_modified = false;
};

} // namespace chip8

static_assert(sizeof(chip8::cpu_t) == 6200);