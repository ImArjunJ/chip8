#include "chip8/chip8.hpp"
#include <iostream>
#include <print>
#include <utility>

using namespace std::chrono_literals;

const std::uint32_t TARGET_CPU_HZ = 500;
const std::uint32_t TARGET_TIMER_HZ = 60;

int main(int argc, char **argv) {
  if (argc < 2) {
    std::println(std::cerr, "usage chip8 <file.ch8>");
    return -1;
  }
  chip8::emulator emulator;
  auto status = emulator.load_cartridge(argv[1]);

  if (!status) {
    std::println("load_cartridge error: {}",
                 std::to_underlying(status.error()));
    return -1;
  }

  std::jthread execution_thread([&] {
    while (emulator.is_running()) {
      auto wait_until =
          std::chrono::steady_clock::now() + (1000000us / TARGET_CPU_HZ);
      emulator.step_instruction();
      std::this_thread::sleep_until(wait_until);
    }
  });
  chip8::window window{{1280, 720}};
  while (window.is_running() && emulator.is_running()) {
    auto wait_until =
        std::chrono::steady_clock::now() + (1000000us / TARGET_TIMER_HZ);

    window.handle_events();

    if (emulator.get_sound_timer()) {
      window.start_sound();
    } else {
      window.stop_sound();
    }

    for (const auto &[sf_key, chip8_key] : chip8::key_map) {
      emulator.set_key(sf_key, window.get_keys()[static_cast<size_t>(sf_key)]);
    }

    emulator.decrement_timers();

    window.render_ui([] { ImGui::ShowDemoWindow(); });

    emulator.render(window);

    std::this_thread::sleep_until(wait_until);
  }

  emulator.stop();

  return 0;
}
