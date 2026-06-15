#pragma once
#include "window.hpp"
#include <array>
#include <cstdint>
#include <expected>
#include <string_view>
#include <thread>
#include <unordered_map>

namespace chip8 {
enum class load_error : std::uint8_t { FILE_ERROR, ROM_TOO_LARGE };

enum class key_type : std::uint16_t {
  ZERO = 0x0,
  ONE = 0x1,
  TWO = 0x2,
  THREE = 0x3,
  FOUR = 0x4,
  FIVE = 0x5,
  SIX = 0x6,
  SEVEN = 0x7,
  EIGHT = 0x8,
  NINE = 0x9,
  A = 0xA,
  B = 0xB,
  C = 0xC,
  D = 0xD,
  E = 0xE,
  F = 0xF,
  NONE
};

inline std::unordered_map<sf::Keyboard::Key, key_type> key_map = {
    {sf::Keyboard::Num1, key_type::ONE},   {sf::Keyboard::Num2, key_type::TWO},
    {sf::Keyboard::Num3, key_type::THREE}, {sf::Keyboard::Num4, key_type::C},
    {sf::Keyboard::Q, key_type::FOUR},     {sf::Keyboard::W, key_type::FIVE},
    {sf::Keyboard::E, key_type::SIX},      {sf::Keyboard::R, key_type::D},
    {sf::Keyboard::A, key_type::SEVEN},    {sf::Keyboard::S, key_type::EIGHT},
    {sf::Keyboard::D, key_type::NINE},     {sf::Keyboard::F, key_type::E},
    {sf::Keyboard::Z, key_type::A},        {sf::Keyboard::X, key_type::ZERO},
    {sf::Keyboard::C, key_type::B},        {sf::Keyboard::V, key_type::F}};

struct key_t {
  key_t() = default;
  key_t(sf::Keyboard::Key key) {
    if (key_map.contains(key)) {
      this->code = key_map.at(key);
      this->pressed = true;
    } else {
      this->code = key_type::NONE;
    }
  }
  bool pressed{};
  key_type code{key_type::NONE};
};

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
  std::array<bool, 16> keys{};
  static constexpr std::uint16_t FONT_START_ADDRESS = 0x050;
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
  void set_key(key_t key, bool state);
  void decrement_timers();
  std::uint8_t get_sound_timer();

private:
  chip8::cpu_t cpu{};
  bool running = true;
  bool pc_modified = false;
};

} // namespace chip8
