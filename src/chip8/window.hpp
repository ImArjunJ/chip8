#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Audio/SoundSource.hpp>
#include <SFML/Graphics.hpp>
#include <array>
namespace chip8 {
class window {
public:
  window(sf::Vector2u size = {640, 320}) {
    m_window.create(sf::VideoMode{size.x, size.y}, "chip8", sf::Style::None);
    m_window.setFramerateLimit(60);

    m_buzzer = this->create_buzzer();
  }
  bool is_running() { return m_window.isOpen(); };
  void handle_events() {
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
  std::array<bool, 102> get_keys() { return m_keys; }
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

  void start_sound() {
    if (m_buzzer.getStatus() != sf::SoundSource::Playing) {
      m_buzzer.play();
    }
  }

  void stop_sound() { m_buzzer.stop(); }

private:
  sf::Sound create_buzzer() {
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

private:
  sf::Sound m_buzzer{};
  sf::RenderWindow m_window;
  std::array<bool, 102> m_keys = {};
};
} // namespace chip8