#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Audio/SoundSource.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include <functional>
#include <imgui.h>

namespace chip8 {
class window {
public:
  window(sf::Vector2u size = {1280, 720});
  bool is_running();
  void handle_events();
  std::array<bool, 102> get_keys();
  void render(std::array<std::array<std::uint8_t, 64>, 32> buffer);
  void start_sound();
  void stop_sound();
  void render_ui(std::function<void()> callback);

private:
  void create_buzzer();
  void apply_theme();
  void build_default_layout(ImGuiID dockspace_id);

  sf::SoundBuffer m_buffer;
  sf::Sound m_buzzer;
  sf::RenderWindow m_window;
  sf::Clock m_clock;
  sf::Texture m_display_texture;
  std::array<std::uint8_t, 64 * 32 * 4> m_pixels{};
  std::array<bool, 102> m_keys{};
  bool m_first_frame{true};
};
} // namespace chip8
