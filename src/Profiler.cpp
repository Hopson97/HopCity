#include "Profiler.h"

Profiler::Profiler() { m_last50times.resize(50); }

void Profiler::reset()
{
    auto time = m_internalClock.restart();

    frametime = time.asSeconds();
    m_last50times.push_back(frametime);
    if (m_last50times.size() > 50)
    {
        m_last50times.pop_front();
    }

    float tmpavg = 0;
    for (auto& x : m_last50times)
    {
        tmpavg += x;
    }
    frametimeAverage = tmpavg / 0.05f;

    for (auto& x : m_activeslots)
    {

        if (!x.stopped)
        {
            x.stop();
            m_lastSlots.emplace_back(std::make_pair("ForgotToStop: " + x.name, 0.f));
            continue;
        }
        m_lastSlots.emplace_back(std::make_pair(x.name, Mode == ProfilerMode::Milliseconds
                                                            ? x.getTime().asSeconds() * 1000
                                                            : x.getTime().asSeconds() * 1000 * 1000));
    }
    m_activeslots.clear();
}

void Profiler::onGUI()
{
    ImGui::SetNextWindowBgAlpha(0.7f);
    if (ImGui::Begin("Profiler", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
    {
        ImGui::TextColored({0, 0.392f, 0.960f, 1}, "Current Frametime: %f ms",
                           static_cast<float>(frametime * 1000.0f));

        std::array<float, 50> temptimes;
        for (unsigned x = 0; x < temptimes.size(); x++)
        {
            temptimes[x] = m_last50times[x];
        }
        ImGui::PlotHistogram("##Times", temptimes.data(), m_last50times.size());
        ImGui::Text("Avg: %.2f ms,(%.1f fps)", frametimeAverage, 1000.f / frametimeAverage);
        ImGui::Separator();
        ImGui::TextColored({0, 0.6f, 0.188f, 1}, "Times in %s",
                           Mode == ProfilerMode::Milliseconds ? "Milliseconds" : "Microseconds");
        for (auto& ts : m_lastSlots)
        {
            ImGui::Text("%s: %0.2f", ts.first.c_str(), ts.second);
        }
    }
    ImGui::End();
    m_lastSlots.clear();
}

TimeSlot& Profiler::newTimeslot(const char* name) { return m_activeslots.emplace_back(name); }