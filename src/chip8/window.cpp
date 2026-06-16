#include "window.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <iostream>
#include <print>

namespace chip8 {
window::window(sf::Vector2u size) : m_buzzer(m_buffer) {
  m_window.create(sf::VideoMode({size.x, size.y}), "chip8", sf::Style::None);
  this->create_buzzer();
  m_buzzer.setBuffer(m_buffer);
  m_buzzer.setLooping(true);
}

bool window::is_running() { return m_window.isOpen(); }

void window::handle_events() {
  while (const std::optional<sf::Event> event = m_window.pollEvent()) {
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
        sf::RectangleShape pixel({10.f, 10.f});
        pixel.setPosition({pixel.getSize().x * x, pixel.getSize().y * y});
        pixel.setFillColor(sf::Color::White);
        m_window.draw(pixel);
      }
    }
  }
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
