#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Audio/SoundSource.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include <imgui.h>
namespace chip8 {
class window {
public:
  window(sf::Vector2u size = {640, 320});
  bool is_running();
  void handle_events();
  std::array<bool, 102> get_keys();
  void render(std::array<std::array<std::uint8_t, 64>, 32> buffer);
  void start_sound();
  void stop_sound();
  void render_ui(std::function<void()> callback);

private:
  void create_buzzer();

private:
  sf::SoundBuffer m_buffer;
  sf::Sound m_buzzer;
  sf::RenderWindow m_window;
  sf::Clock m_clock;
  std::array<bool, 102> m_keys = {};
};
} // namespace chip8