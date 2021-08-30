#include "Animation.h"

Animation::Animation(int frameWidth, int frameHeight, bool isRandom, float randFactor)
    : FRAME_WIDTH(frameWidth)
    , FRAME_HEIGHT(frameHeight)

    , m_isRandom(isRandom)
    , m_randomFactor(randFactor)
{
}

void Animation::addFrame(int row, int index, sf::Time delay)
{
    sf::IntRect bounds;
    bounds.top = FRAME_HEIGHT * row;
    bounds.height = FRAME_HEIGHT;
    bounds.width = FRAME_WIDTH;
    bounds.left = index * FRAME_WIDTH;

    m_frames.emplace_back(bounds, delay);
}

const sf::IntRect& Animation::getCurrentFrame() { return m_frames[m_framePointer].bounds; }

const sf::IntRect& Animation::progressFrame()
{
    if (m_isRandom) {
        if (rand() % 1024 < (int)(m_randomFactor * 1024.0f) && !m_randomAnimationIsRunning) {
            m_randomAnimationIsRunning = true; // switch on one round
        }
        if (!m_randomAnimationIsRunning) {
            m_timer.restart();
            return m_frames[0].bounds;
        }
    }

    m_overlappedTime += m_timer.getElapsedTime();
    while (m_overlappedTime >= m_frames[m_framePointer].delay) {
        m_overlappedTime -= m_frames[m_framePointer].delay;
        m_framePointer++;
        if (m_framePointer == m_frames.size()) {
            m_framePointer = 0;
            m_randomAnimationIsRunning = false;
        }
    }
    m_timer.restart();
    return m_frames[m_framePointer].bounds;
}

bool Animation::isOnLastFrame() const { return m_framePointer == m_frames.size() - 1; }

void Animation::reset()
{
    m_framePointer = 0;
    m_timer.restart();
}
