#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Clock.hpp>
#include <vector>

class Animation {
  public:
    struct Frame {
        Frame(const sf::IntRect& bounds, sf::Time delay)
            : bounds(bounds)
            , delay(delay)
        {
        }

        sf::IntRect bounds;
        sf::Time delay;
    };

    Animation(int frameWidth, int frameHeight,bool is_random= false,float random_factor=0.01);

    void addFrame(int row, int index, sf::Time delay);

    const sf::IntRect& getFrame();

  private:
    sf::Clock m_timer;
    sf::Time m_overlappedTime;

    std::vector<Frame> m_frames;
    float m_random_factor=0.f;
    bool m_israndom, random_animation_running = false;
    int m_framePointer = 0;
    const int FRAME_WIDTH;
    const int FRAME_HEIGHT;
};