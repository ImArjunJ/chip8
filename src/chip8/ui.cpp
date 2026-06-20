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

  if (ImGui::BeginTable("regs", 4,
                        ImGuiTableFlags_BordersInnerH |
                            ImGuiTableFlags_BordersInnerV |
                            ImGuiTableFlags_SizingStretchSame)) {
    for (int i = 0; i < 16; i++) {
      ImGui::TableNextColumn();
      ImGui::TextDisabled("V%X", i);
      ImGui::SameLine();
      ImGui::Text("%02X", cpu.registers[i]);
    }
    ImGui::EndTable();
  }

  ImGui::Spacing();

  if (ImGui::BeginTable("state", 5,
                        ImGuiTableFlags_BordersInnerV |
                            ImGuiTableFlags_SizingStretchSame)) {
    ImGui::TableNextColumn();
    ImGui::TextDisabled("PC");
    ImGui::SameLine();
    ImGui::Text("%04X", cpu.pc);
    ImGui::TableNextColumn();
    ImGui::TextDisabled("I");
    ImGui::SameLine();
    ImGui::Text("%04X", cpu.i);
    ImGui::TableNextColumn();
    ImGui::TextDisabled("SP");
    ImGui::SameLine();
    ImGui::Text("%02X", cpu.sp);
    ImGui::TableNextColumn();
    ImGui::TextDisabled("DT");
    ImGui::SameLine();
    ImGui::Text("%02X", cpu.delay_timer);
    ImGui::TableNextColumn();
    ImGui::TextDisabled("ST");
    ImGui::SameLine();
    ImGui::Text("%02X", cpu.sound_timer);
    ImGui::EndTable();
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::BeginChild("##memory", ImVec2(0, 0), ImGuiChildFlags_None,
                    ImGuiWindowFlags_None);

  constexpr int bytes_per_row = 16;
  constexpr int total_rows = 4096 / bytes_per_row;

  ImGuiListClipper clipper;
  clipper.Begin(total_rows);

  while (clipper.Step()) {
    for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
      int addr = row * bytes_per_row;

      ImGui::TextDisabled("%03X:", addr);
      ImGui::SameLine();

      for (int col = 0; col < bytes_per_row; col++) {
        int byte_addr = addr + col;
        bool is_pc = (byte_addr == cpu.pc || byte_addr == cpu.pc + 1);
        bool is_i = (byte_addr >= cpu.i && byte_addr < cpu.i + 16 && cpu.i != 0);

        if (is_pc)
          ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        else if (is_i)
          ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));

        ImGui::SameLine(0, col == 0 ? 4 : 0);
        ImGui::Text("%02X", cpu.memory[byte_addr]);

        if (is_pc || is_i)
          ImGui::PopStyleColor();

        if (col == 7) {
          ImGui::SameLine(0, 0);
          ImGui::TextDisabled(" ");
        }
      }

      ImGui::SameLine(0, 8);
      char ascii[bytes_per_row + 1];
      for (int col = 0; col < bytes_per_row; col++) {
        std::uint8_t b = cpu.memory[addr + col];
        ascii[col] = (b >= 0x20 && b < 0x7F) ? static_cast<char>(b) : '.';
      }
      ascii[bytes_per_row] = '\0';
      ImGui::TextDisabled("%s", ascii);
    }
  }

  clipper.End();
  ImGui::EndChild();
  ImGui::End();
}

void disassembly(const cpu_t &cpu) {
  ImGui::Begin("Disassembly");

  constexpr int context_lines = 32;
  constexpr std::uint16_t rom_start = 0x200;

  int start_addr = std::max(static_cast<int>(rom_start),
                            static_cast<int>(cpu.pc) - context_lines * 2);
  start_addr &= ~1;
  int end_addr = std::min(4095, static_cast<int>(cpu.pc) + context_lines * 2);

  for (int addr = start_addr; addr < end_addr; addr += 2) {
    if (addr + 1 >= static_cast<int>(cpu.memory.size()))
      break;

    auto opcode = static_cast<std::uint16_t>(
        (static_cast<std::uint16_t>(cpu.memory[addr]) << 8) |
        static_cast<std::uint16_t>(cpu.memory[addr + 1]));

    instruction_t ins(opcode);
    char mnemonic[32];
    disassemble(ins, mnemonic, sizeof(mnemonic));

    bool is_pc = (addr == static_cast<int>(cpu.pc));

    if (is_pc) {
      ImVec2 pos = ImGui::GetCursorScreenPos();
      ImVec2 row_size(ImGui::GetContentRegionAvail().x,
                      ImGui::GetTextLineHeightWithSpacing());
      ImGui::GetWindowDrawList()->AddRectFilled(
          pos, ImVec2(pos.x + row_size.x, pos.y + row_size.y),
          IM_COL32(180, 180, 180, 255));
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    }

    ImGui::Text("%04X  %04X  %-16s", addr, opcode, mnemonic);

    if (is_pc) {
      ImGui::PopStyleColor();
    }
  }

  ImGui::End();
}

} // namespace chip8::ui
