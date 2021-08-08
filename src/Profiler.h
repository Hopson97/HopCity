#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui_sfml/imgui-SFML.h>
#include <imgui_sfml/imgui.h>
//cross platform memory usage thingy

class TimeSlot{
  public:
    TimeSlot(std::string _name): name(_name){
        begin = _internalClock.restart();
        end = begin;

    }

    void stop(){
        end = _internalClock.restart();
        stopped= true;
    }

    sf::Time getTime(){
        return end-begin;
    }

    bool stopped = false;
    std::string name;
  private:
    sf::Time begin,end;
    sf::Clock _internalClock;
};

//Small Profiler that can be used to track different parts of the code
class Profiler {
  public:
    enum ProfilerMode{MILLIS,MICRO};
    Profiler(){
        _last50times.resize(50);
    };
    ProfilerMode Mode = MICRO;
    void reset(){

        auto temp =_internalClock.restart();

        Frametime = temp.asSeconds();
        FrametimeMicro = temp.asMicroseconds();
        FrametimeMilli = temp.asMilliseconds();
        _last50times.push_back(Frametime);
        if(_last50times.size()>50){_last50times.pop_front();}

        float tmpavg;
        for(auto& x:_last50times){tmpavg+=x;}
        FrametimeAverage = tmpavg/0.05;

        for(auto& x:_activeslots){

            if(!x.stopped){
                x.stop();
                _lastSlots.emplace_back(std::make_pair("ForgotToStop: "+x.name,0.f));
                continue;
            }
            _lastSlots.emplace_back(std::make_pair(x.name,Mode==MILLIS?x.getTime().asMilliseconds():x.getTime().asMicroseconds()));
        }
        _activeslots.clear();
    };
    void onGUI(){
        if (ImGui::Begin("Profiler")) {
            ImGui::TextColored({0, 0.392, 0.960,1},("Current Frametime:"+std::to_string(FrametimeMilli)+"ms").c_str());
            std::array<float,50> _temptimes;
            for(int x =0;x<_temptimes.size();x++){
                _temptimes[x]=_last50times[x];
            }
            ImGui::PlotHistogram("##Times",_temptimes.data(),_last50times.size());
            ImGui::Text("Avg: %.2f ms,(%.1f fps)",FrametimeAverage,1000.f/FrametimeAverage);
            ImGui::Separator();
            ImGui::TextColored({0, 0.6, 0.188,1},"Times in %s", Mode==MILLIS?"Milliseconds":"Microseconds");
            std::vector<float> temptime;
            for(auto& ts : _lastSlots){
                ImGui::Text((ts.first+":"+std::to_string(ts.second)).c_str());
                temptime.emplace_back(ts.second);
            }
            ImGui::PlotHistogram("Times",temptime.data(),temptime.size());
        }
        ImGui::End();
        _lastSlots.clear();
    };
    TimeSlot& newTimeslot(std::string name){
        return _activeslots.emplace_back(name);
    };
    int FrametimeMilli;
    int FrametimeMicro;
    float Frametime;
    float FrametimeAverage;
  private:
    std::deque<float> _last50times;
    std::vector<TimeSlot> _activeslots;
    std::vector<std::pair<std::string,float>> _lastSlots;
    sf::Clock _internalClock;
};
