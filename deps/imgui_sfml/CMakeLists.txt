add_library(imgui_sfml
    imgui-SFML.cpp
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_widgets.cpp
)


target_link_libraries(imgui_sfml 
    ${CONAN_LIBS}
)