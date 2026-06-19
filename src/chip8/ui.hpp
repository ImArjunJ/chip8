#pragma once
#include "chip8.hpp"

namespace chip8::ui {

void cpu_state(const cpu_t &cpu);
void disassembly(const cpu_t &cpu);

} // namespace chip8::ui
