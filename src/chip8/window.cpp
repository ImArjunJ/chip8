#define IMGUI_DEFINE_MATH_OPERATORS
#include "window.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include <print>

namespace chip8 {

window::window(sf::Vector2u size) : m_buzzer(m_buffer) {
  m_window.create(sf::VideoMode({size.x, size.y}), "chip8", sf::Style::Default);
  create_buzzer();
  m_buzzer.setBuffer(m_buffer);
  m_buzzer.setLooping(true);

  if (!ImGui::SFML::Init(m_window)) {
    std::println(std::cerr, "failed to init imgui-sfml");
  }

  auto &io = ImGui::GetIO();
  io.IniFilename = nullptr;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  apply_theme();

  if (!m_display_texture.resize({64, 32})) {
    std::println(std::cerr, "display texture resize failed");
  }
  m_display_texture.setSmooth(false);
}

bool window::is_running() { return m_window.isOpen(); }

void window::handle_events() {
  while (const std::optional<sf::Event> event = m_window.pollEvent()) {
    ImGui::SFML::ProcessEvent(m_window, *event);
    if (event->is<sf::Event::Closed>()) {
      m_window.close();
    } else if (const auto *key = event->getIf<sf::Event::KeyPressed>()) {
      auto code = static_cast<std::size_t>(key->code);
      if (code < m_keys.size())
        m_keys[code] = true;
    } else if (const auto *key = event->getIf<sf::Event::KeyReleased>()) {
      auto code = static_cast<std::size_t>(key->code);
      if (code < m_keys.size())
        m_keys[code] = false;
    }
  }
}

std::array<bool, 102> window::get_keys() { return m_keys; }

void window::render_ui(std::function<void()> callback) {
  ImGui::SFML::Update(m_window, m_clock.restart());
  ImGuiID dockspace_id =
      ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_NoUndocking);

  if (m_first_frame) {
    build_default_layout(dockspace_id);
    m_first_frame = false;
  }

  callback();
}

void window::build_default_layout(ImGuiID dockspace_id) {
  ImGui::DockBuilderRemoveNode(dockspace_id);
  ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
  ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

  ImGuiID right;
  ImGuiID left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left,
                                             0.5f, nullptr, &right);

  ImGui::DockBuilderDockWindow("Display", left);
  ImGui::DockBuilderDockWindow("Disassembly", right);
  ImGui::DockBuilderDockWindow("CPU", right);

  ImGui::DockBuilderGetNode(left)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
  ImGui::DockBuilderGetNode(right)->LocalFlags |=
      ImGuiDockNodeFlags_NoWindowMenuButton;

  ImGui::DockBuilderFinish(dockspace_id);
}

void window::render(std::array<std::array<std::uint8_t, 64>, 32> buffer) {
  constexpr std::uint32_t width = 64;
  constexpr std::uint32_t height = 32;

  for (std::uint32_t y = 0; y < height; y++) {
    for (std::uint32_t x = 0; x < width; x++) {
      auto index = (y * width + x) * 4;
      std::uint8_t value = buffer[y][x] ? 255 : 0;
      m_pixels[index + 0] = value;
      m_pixels[index + 1] = value;
      m_pixels[index + 2] = value;
      m_pixels[index + 3] = 255;
    }
  }
  m_display_texture.update(m_pixels.data());

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  ImGui::Begin("Display");
  ImVec2 avail = ImGui::GetContentRegionAvail();
  float scale = std::min(avail.x / width, avail.y / height);
  ImVec2 size(width * scale, height * scale);
  ImVec2 cursor = ImGui::GetCursorPos();
  cursor.x += (avail.x - size.x) * 0.5f;
  cursor.y += (avail.y - size.y) * 0.5f;
  ImGui::SetCursorPos(cursor);
  ImGui::Image(m_display_texture, size);
  ImGui::End();
  ImGui::PopStyleVar();

  m_window.clear();
  ImGui::SFML::Render();
  m_window.display();
}

void window::start_sound() {
  if (m_buzzer.getStatus() != sf::SoundSource::Status::Playing) {
    m_buzzer.play();
  }
}

void window::stop_sound() { m_buzzer.stop(); }

void window::create_buzzer() {
  std::vector<std::int16_t> samples(44100 / 10);
  for (std::size_t i = 0; i < samples.size(); ++i) {
    samples[i] = ((i / 50) % 2 == 0) ? 5000 : -5000;
  }

  const std::vector<sf::SoundChannel> channel_map = {sf::SoundChannel::Mono};
  if (!m_buffer.loadFromSamples(samples.data(), samples.size(), 1, 44100,
                                channel_map)) {
    std::println(std::cerr, "loadFromSamples failed");
  }
}

void window::apply_theme() {
  auto &style = ImGui::GetStyle();
  style.WindowRounding = 0.0f;
  style.FrameRounding = 0.0f;
  style.GrabRounding = 0.0f;
  style.TabRounding = 0.0f;
  style.ScrollbarRounding = 0.0f;
  style.ChildRounding = 0.0f;
  style.PopupRounding = 0.0f;
  style.WindowBorderSize = 1.0f;
  style.FrameBorderSize = 0.0f;
  style.TabBorderSize = 0.0f;
  style.TabBarBorderSize = 1.0f;
  style.TabBarOverlineSize = 0.0f;
  style.WindowPadding = ImVec2(4, 4);
  style.FramePadding = ImVec2(4, 2);
  style.ItemSpacing = ImVec2(4, 2);
  style.ItemInnerSpacing = ImVec2(4, 2);
  style.CellPadding = ImVec2(4, 1);
  style.IndentSpacing = 12.0f;
  style.ScrollbarSize = 8.0f;
  style.GrabMinSize = 6.0f;
  style.SeparatorTextBorderSize = 1.0f;

  auto *c = style.Colors;
  c[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  c[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  c[ImGuiCol_PopupBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  c[ImGuiCol_Border] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
  c[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  c[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  c[ImGuiCol_FrameBgHovered] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
  c[ImGuiCol_FrameBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
  c[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  c[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  c[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  c[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  c[ImGuiCol_ScrollbarBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  c[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
  c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
  c[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
  c[ImGuiCol_Header] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
  c[ImGuiCol_HeaderHovered] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
  c[ImGuiCol_HeaderActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
  c[ImGuiCol_Separator] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
  c[ImGuiCol_SeparatorHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
  c[ImGuiCol_SeparatorActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
  c[ImGuiCol_ResizeGrip] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
  c[ImGuiCol_ResizeGripHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
  c[ImGuiCol_ResizeGripActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
  c[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  c[ImGuiCol_TabHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  c[ImGuiCol_TabSelected] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  c[ImGuiCol_TabSelectedOverline] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
  c[ImGuiCol_TabDimmed] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  c[ImGuiCol_TabDimmedSelected] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  c[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  c[ImGuiCol_DockingPreview] = ImVec4(0.30f, 0.30f, 0.30f, 0.70f);
  c[ImGuiCol_DockingEmptyBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  c[ImGuiCol_Button] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  c[ImGuiCol_ButtonHovered] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
  c[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
  c[ImGuiCol_Text] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
  c[ImGuiCol_TextDisabled] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
  c[ImGuiCol_CheckMark] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
  c[ImGuiCol_SliderGrab] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
  c[ImGuiCol_SliderGrabActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
}

} // namespace chip8
