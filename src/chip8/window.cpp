#define IMGUI_DEFINE_MATH_OPERATORS
#include "window.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <array>
#include <functional>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>
#include <print>

namespace chip8 {
window::window(sf::Vector2u size) : m_buzzer(m_buffer) {
  m_window.create(sf::VideoMode({size.x, size.y}), "chip8", sf::Style::None);
  this->create_buzzer();
  m_buzzer.setBuffer(m_buffer);
  m_buzzer.setLooping(true);
  if (!ImGui::SFML::Init(m_window)) {
    std::println(std::cerr, "failed to init imgui-sfml");
  }
  ImGui::GetIO().IniFilename = NULL;
}

bool window::is_running() { return m_window.isOpen(); }

void window::handle_events() {
  while (const std::optional<sf::Event> event = m_window.pollEvent()) {
    ImGui::SFML::ProcessEvent(m_window, *event);
    if (event->is<sf::Event::Closed>()) {
      m_window.close();
    } else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
      m_keys[static_cast<std::size_t>(keyPressed->code)] = true;
    } else if (const auto *keyReleased =
                   event->getIf<sf::Event::KeyReleased>()) {
      m_keys[static_cast<std::size_t>(keyReleased->code)] = false;
    }
  }
}

std::array<bool, 102> window::get_keys() { return m_keys; }

void window::render_ui(std::function<void()> callback) {
  ImGui::SFML::Update(m_window, m_clock.restart());
  callback();
}

void window::render(std::array<std::array<std::uint8_t, 64>, 32> buffer) {
  constexpr const std::uint32_t width = 64;
  constexpr const std::uint32_t height = 32;
  sf::Texture display_texture{};
  if (!display_texture.resize({width, height})) {
    std::println(std::cerr, "resize failed");
  }
  display_texture.setSmooth(false);
  std::array<std::uint8_t, width * height * 4> pixels{};
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int index = (y * width + x) * 4;
      if (buffer[y][x] == 1) {
        pixels[index + 0] = 255; // R
        pixels[index + 1] = 255; // G
        pixels[index + 2] = 255; // B
        pixels[index + 3] = 255; // A
      } else {
        pixels[index + 0] = 0;   // R
        pixels[index + 1] = 0;   // G
        pixels[index + 2] = 0;   // B
        pixels[index + 3] = 255; // A
      }
    }
  }
  display_texture.update(pixels.data());
  ImGui::SetNextWindowSize({640, 320}, ImGuiCond_FirstUseEver);
  ImGui::Begin("chip8 display");
  ImGui::Image(display_texture, ImGui::GetContentRegionAvail());
  ImGui::End();
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
  for (size_t i = 0; i < samples.size(); ++i) {
    samples[i] = ((i / 50) % 2 == 0) ? 5000 : -5000;
  }

  const std::vector<sf::SoundChannel> channel_map = {sf::SoundChannel::Mono};

  auto result = m_buffer.loadFromSamples(samples.data(), samples.size(), 1,
                                         44100, channel_map);
  if (!result) {
    std::println(std::cerr, "loadFromSamples failed");
  }
}
} // namespace chip8
