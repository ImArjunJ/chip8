#pragma once
#include <SFML/Graphics.hpp>

namespace chip8 {
class window {
public:
  window(sf::Vector2u size = {640, 320}) {
    m_window.create(sf::VideoMode{size.x, size.y}, "chip8", sf::Style::None);
  }
  bool is_running() { return m_window.isOpen(); };
  void handle_events() {
    sf::Event e;
    while (m_window.pollEvent(e)) {
      if (e.type == sf::Event::Closed) {
        m_window.close();
      }
    }
  }
  void render(std::array<std::array<std::uint8_t, 64>, 32> buffer) {
    m_window.clear();
    {
      for (std::uint32_t y = 0; y < buffer.size(); ++y) {
        for (std::uint32_t x = 0; x < buffer[0].size(); ++x) {
          if (!buffer[y][x])
            continue;
          sf::RectangleShape pixel({10, 10}); // TODO: update rect size
          pixel.setPosition(pixel.getSize().x * x, pixel.getSize().y * y);
          pixel.setFillColor(sf::Color::White);
          m_window.draw(pixel);
        }
      }
    }
    m_window.display();
  }

private:
  sf::RenderWindow m_window;
};
} // namespace chip8