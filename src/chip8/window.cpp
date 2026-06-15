#include "window.hpp"

namespace chip8 {
window::window(sf::Vector2u size) {
  m_window.create(sf::VideoMode{size.x, size.y}, "chip8", sf::Style::None);

  m_buzzer = this->create_buzzer();
}
bool window::is_running() { return m_window.isOpen(); };
void window::handle_events() {
  sf::Event e;
  while (m_window.pollEvent(e)) {
    if (e.type == sf::Event::Closed) {
      m_window.close();
    } else if (e.type == sf::Event::KeyPressed) {
      m_keys[e.key.code] = true;
    } else if (e.type == sf::Event::KeyReleased) {
      m_keys[e.key.code] = false;
    }
  }
}
std::array<bool, 102> window::get_keys() { return m_keys; }
void window::render(std::array<std::array<std::uint8_t, 64>, 32> buffer) {
  m_window.clear();
  {
    // TODO: render ui;
    // TODO: move chip8 rendering to imgui drawlist
  }
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

void window::start_sound() {
  if (m_buzzer.getStatus() != sf::SoundSource::Playing) {
    m_buzzer.play();
  }
}

void window::stop_sound() { m_buzzer.stop(); }

sf::Sound window::create_buzzer() {
  std::vector<sf::Int16> samples(44100 / 10);
  for (size_t i = 0; i < samples.size(); ++i) {
    samples[i] = ((i / 50) % 2 == 0) ? 5000 : -5000;
  }

  static sf::SoundBuffer buffer;
  buffer.loadFromSamples(samples.data(), samples.size(), 1, 44100);

  sf::Sound buzzer;
  buzzer.setBuffer(buffer);
  buzzer.setLoop(true);
  return buzzer;
}
} // namespace chip8