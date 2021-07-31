#include "Animation.h"

Animation::Animation(unsigned frameWidth, unsigned frameHeight)
    : FRAME_WIDTH(frameWidth)
    , FRAME_HEIGHT(frameHeight)
{
}

void Animation::addFrame(unsigned row, unsigned index, sf::Time delay)
{
    sf::IntRect bounds;
    bounds.top = FRAME_HEIGHT * row;
    bounds.height = FRAME_HEIGHT;
    bounds.width = FRAME_WIDTH;
    bounds.left = index * FRAME_WIDTH;

    m_frames.emplace_back(bounds, delay);
}

const sf::IntRect& Animation::getFrame()
{
    m_overlappedTime += m_timer.getElapsedTime();
    while (m_overlappedTime >= m_frames[m_framePointer].delay) {
        m_overlappedTime -= m_frames[m_framePointer].delay;
        m_framePointer++;
        if (m_framePointer == m_frames.size()) {
            m_framePointer = 0;
        }
    }
    m_timer.restart();
    return m_frames[m_framePointer].bounds;
}
