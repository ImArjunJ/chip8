#include "chip8/chip8.hpp"
#include <chrono>
#include <print>
#include <thread>
#include <utility>
/*
16-key hexadecimal keypad layout:
1	2	3	C
4	5	6	D
7	8	9	E
A	0	B	F
*/

int main(int, char **) {
  // TODO: cli setup? arguments?
  chip8::emulator emulator;
  auto status = emulator.load_cartridge("roms/ibm.ch8");
  if (!status) {
    std::println("load_cartridge error: {}",
                 std::to_underlying(status.error()));
    return -1;
  }
  std::thread execution_thread([&] {
    while (emulator.is_running()) {
      if (!emulator.step_instruction()) {
        emulator.stop();
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
  });
  execution_thread.detach();

  chip8::window window;
  while (window.is_running() && emulator.is_running()) {
    window.handle_events();
    emulator.render(window);
  }

  emulator.stop();

  if (execution_thread.joinable()) {
    execution_thread.join();
  }

  return 0;
}
