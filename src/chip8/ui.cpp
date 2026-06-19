#include "ui.hpp"
#include "instruction.hpp"
#include <cstdio>
#include <imgui.h>

namespace chip8::ui {

static const char *disassemble(instruction_t ins, char *buf, std::size_t len) {
  switch (ins.type) {
  case instruction_type::CLS:
    std::snprintf(buf, len, "CLS");
    break;
  case instruction_type::RET:
    std::snprintf(buf, len, "RET");
    break;
  case instruction_type::SYS:
    std::snprintf(buf, len, "SYS  %03X", ins.nnn);
    break;
  case instruction_type::JP:
    std::snprintf(buf, len, "JP   %03X", ins.nnn);
    break;
  case instruction_type::CALL:
    std::snprintf(buf, len, "CALL %03X", ins.nnn);
    break;
  case instruction_type::SE_VX_BYTE:
    std::snprintf(buf, len, "SE   V%X, %02X", ins.x, ins.kk);
    break;
  case instruction_type::SNE_VX_BYTE:
    std::snprintf(buf, len, "SNE  V%X, %02X", ins.x, ins.kk);
    break;
  case instruction_type::SE_VX_VY:
    std::snprintf(buf, len, "SE   V%X, V%X", ins.x, ins.y);
    break;
  case instruction_type::LD_VX_BYTE:
    std::snprintf(buf, len, "LD   V%X, %02X", ins.x, ins.kk);
    break;
  case instruction_type::ADD_VX_BYTE:
    std::snprintf(buf, len, "ADD  V%X, %02X", ins.x, ins.kk);
    break;
  case instruction_type::LD_VX_VY:
    std::snprintf(buf, len, "LD   V%X, V%X", ins.x, ins.y);
    break;
  case instruction_type::OR_VX_VY:
    std::snprintf(buf, len, "OR   V%X, V%X", ins.x, ins.y);
    break;
  case instruction_type::AND_VX_VY:
    std::snprintf(buf, len, "AND  V%X, V%X", ins.x, ins.y);
    break;
  case instruction_type::XOR_VX_VY:
    std::snprintf(buf, len, "XOR  V%X, V%X", ins.x, ins.y);
    break;
  case instruction_type::ADD_VX_VY:
    std::snprintf(buf, len, "ADD  V%X, V%X", ins.x, ins.y);
    break;
  case instruction_type::SUB_VX_VY:
    std::snprintf(buf, len, "SUB  V%X, V%X", ins.x, ins.y);
    break;
  case instruction_type::SHR_VX_VY:
    std::snprintf(buf, len, "SHR  V%X", ins.x);
    break;
  case instruction_type::SUBN_VX_VY:
    std::snprintf(buf, len, "SUBN V%X, V%X", ins.x, ins.y);
    break;
  case instruction_type::SHL_VX_VY:
    std::snprintf(buf, len, "SHL  V%X", ins.x);
    break;
  case instruction_type::SNE_VX_VY:
    std::snprintf(buf, len, "SNE  V%X, V%X", ins.x, ins.y);
    break;
  case instruction_type::LD_I_ADDR:
    std::snprintf(buf, len, "LD   I, %03X", ins.nnn);
    break;
  case instruction_type::JP_V0_ADDR:
    std::snprintf(buf, len, "JP   V0, %03X", ins.nnn);
    break;
  case instruction_type::RND_VX_BYTE:
    std::snprintf(buf, len, "RND  V%X, %02X", ins.x, ins.kk);
    break;
  case instruction_type::DRW_VX_VY:
    std::snprintf(buf, len, "DRW  V%X, V%X, %X", ins.x, ins.y, ins.n);
    break;
  case instruction_type::SKP_VX:
    std::snprintf(buf, len, "SKP  V%X", ins.x);
    break;
  case instruction_type::SKNP_VX:
    std::snprintf(buf, len, "SKNP V%X", ins.x);
    break;
  case instruction_type::LD_VX_DT:
    std::snprintf(buf, len, "LD   V%X, DT", ins.x);
    break;
  case instruction_type::LD_VX_K:
    std::snprintf(buf, len, "LD   V%X, K", ins.x);
    break;
  case instruction_type::LD_DT_VX:
    std::snprintf(buf, len, "LD   DT, V%X", ins.x);
    break;
  case instruction_type::LD_ST_VX:
    std::snprintf(buf, len, "LD   ST, V%X", ins.x);
    break;
  case instruction_type::ADD_I_VX:
    std::snprintf(buf, len, "ADD  I, V%X", ins.x);
    break;
  case instruction_type::LD_F_VX:
    std::snprintf(buf, len, "LD   F, V%X", ins.x);
    break;
  case instruction_type::LD_B_VX:
    std::snprintf(buf, len, "LD   B, V%X", ins.x);
    break;
  case instruction_type::LD_I_VX:
    std::snprintf(buf, len, "LD   [I], V%X", ins.x);
    break;
  case instruction_type::LD_VX_I:
    std::snprintf(buf, len, "LD   V%X, [I]", ins.x);
    break;
  default:
    std::snprintf(buf, len, "DW   %04X", ins.instruction);
    break;
  }
  return buf;
}

void cpu_state(const cpu_t &cpu) {
  ImGui::Begin("CPU");

  if (ImGui::CollapsingHeader("Registers", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::BeginTable("regs", 4, ImGuiTableFlags_BordersInnerV);
    ImGui::TableSetupColumn("Reg", ImGuiTableColumnFlags_WidthFixed, 28.0f);
    ImGui::TableSetupColumn("Val", ImGuiTableColumnFlags_WidthFixed, 36.0f);
    ImGui::TableSetupColumn("Reg", ImGuiTableColumnFlags_WidthFixed, 28.0f);
    ImGui::TableSetupColumn("Val", ImGuiTableColumnFlags_WidthFixed, 36.0f);
    ImGui::TableHeadersRow();

    for (int i = 0; i < 8; i++) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::TextDisabled("V%X", i);
      ImGui::TableNextColumn();
      ImGui::Text("%02X", cpu.registers[i]);
      ImGui::TableNextColumn();
      ImGui::TextDisabled("V%X", i + 8);
      ImGui::TableNextColumn();
      ImGui::Text("%02X", cpu.registers[i + 8]);
    }
    ImGui::EndTable();

    ImGui::Spacing();

    ImGui::TextDisabled("PC");
    ImGui::SameLine(40);
    ImGui::Text("%04X", cpu.pc);
    ImGui::TextDisabled("I");
    ImGui::SameLine(40);
    ImGui::Text("%04X", cpu.i);
    ImGui::TextDisabled("SP");
    ImGui::SameLine(40);
    ImGui::Text("%02X", cpu.sp);
  }

  if (ImGui::CollapsingHeader("Timers", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::TextDisabled("Delay");
    ImGui::SameLine(60);
    ImGui::Text("%02X", cpu.delay_timer);
    ImGui::TextDisabled("Sound");
    ImGui::SameLine(60);
    ImGui::Text("%02X", cpu.sound_timer);
  }

  if (ImGui::CollapsingHeader("Stack", ImGuiTreeNodeFlags_DefaultOpen)) {
    if (cpu.sp == 0) {
      ImGui::TextDisabled("(empty)");
    } else {
      for (int i = cpu.sp - 1; i >= 0; i--) {
        if (i == cpu.sp - 1) {
          ImGui::Text("> [%2d] %04X", i, cpu.stack[i]);
        } else {
          ImGui::TextDisabled("  [%2d] %04X", i, cpu.stack[i]);
        }
      }
    }
  }

  ImGui::End();
}

void disassembly(const cpu_t &cpu) {
  ImGui::Begin("Disassembly");

  constexpr int visible_before = 16;
  constexpr int visible_after = 32;
  constexpr std::uint16_t rom_start = 0x200;

  int start_addr =
      std::max(static_cast<int>(rom_start),
               static_cast<int>(cpu.pc) - visible_before * 2);
  start_addr &= ~1;

  int end_addr = std::min(4095, static_cast<int>(cpu.pc) + visible_after * 2);

  ImGui::BeginChild("disasm_scroll", ImVec2(0, 0), ImGuiChildFlags_None,
                    ImGuiWindowFlags_None);

  ImGuiListClipper clipper;
  int total_lines = (end_addr - start_addr) / 2;
  clipper.Begin(total_lines);

  while (clipper.Step()) {
    for (int line = clipper.DisplayStart; line < clipper.DisplayEnd; line++) {
      std::uint16_t addr =
          static_cast<std::uint16_t>(start_addr + line * 2);

      if (addr + 1 >= cpu.memory.size())
        break;

      auto opcode = static_cast<std::uint16_t>(
          (static_cast<std::uint16_t>(cpu.memory[addr]) << 8) |
          static_cast<std::uint16_t>(cpu.memory[addr + 1]));

      instruction_t ins(opcode);
      char mnemonic[32];
      disassemble(ins, mnemonic, sizeof(mnemonic));

      bool is_pc = (addr == cpu.pc);

      if (is_pc) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 size(ImGui::GetContentRegionAvail().x,
                    ImGui::GetTextLineHeightWithSpacing());
        ImGui::GetWindowDrawList()->AddRectFilled(
            pos, ImVec2(pos.x + size.x, pos.y + size.y),
            IM_COL32(255, 255, 255, 255));
      }

      ImGui::Text("%04X  %04X  %s", addr, opcode, mnemonic);

      if (is_pc) {
        ImGui::PopStyleColor();
      }
    }
  }

  clipper.End();
  ImGui::EndChild();
  ImGui::End();
}

} // namespace chip8::ui
