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

    Animation(int frameWidth, int frameHeight);

    void addFrame(int row, int index, sf::Time delay);

    const sf::IntRect& getFrame();

  private:
    sf::Clock m_timer;
    sf::Time m_overlappedTime;

    std::vector<Frame> m_frames;

    int m_framePointer = 0;
    const int FRAME_WIDTH;
    const int FRAME_HEIGHT;
};