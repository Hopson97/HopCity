#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <array>
#include <deque>
#include <imgui_sfml/imgui-SFML.h>
#include <imgui_sfml/imgui.h>

class TimeSlot
{
  public:
    TimeSlot(std::string _name)
        : name(_name)
    {
        begin = _internalClock.restart();
        end = begin;
    }

    void stop()
    {
        end = _internalClock.restart();
        stopped = true;
    }

    sf::Time getTime() { return end - begin; }

    bool stopped = false;
    std::string name;

  private:
    sf::Time begin, end;
    sf::Clock _internalClock;
};

// Small Profiler that can be used to track different parts of the code
class Profiler
{
  public:
    enum class ProfilerMode
    {
        Milliseconds,
        Microseconds,
    };
    Profiler();
    void reset();
    void onGUI();

    TimeSlot& newTimeslot(const char* name);

    float frametime = 0;
    float frametimeAverage = 0;
    ProfilerMode Mode = ProfilerMode::Milliseconds;

  private:
    std::deque<float> m_last50times;
    std::vector<TimeSlot> m_activeslots;
    std::vector<std::pair<std::string, float>> m_lastSlots;
    sf::Clock m_internalClock;
};
