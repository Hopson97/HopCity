#include "Animation.h"

Animation::Animation(int frameWidth, int frameHeight, bool is_random, float random_factor)
    : FRAME_WIDTH(frameWidth)
    , FRAME_HEIGHT(frameHeight)
    , m_israndom(is_random)
    , m_random_factor(random_factor)
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

const sf::IntRect& Animation::getFrame()
{
    if(m_israndom){
        if(rand()%1000<(int)(m_random_factor*1000.f)&&!random_animation_running){
            random_animation_running= true; //switch on one round
        }
        if(!random_animation_running){
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
            random_animation_running = false; //switch off after round is done
        }
    }
    m_timer.restart();
    return m_frames[m_framePointer].bounds;
}
