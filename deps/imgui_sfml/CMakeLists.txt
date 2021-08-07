add_library(imgui_sfml
    imgui-SFML.cpp
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_widgets.cpp
)

if(WIN32)
    target_link_libraries(imgui_sfml sfml-audio sfml-graphics sfml-window sfml-system sfml-network ${OPENGL_LIBRARIES} )
else()
    target_link_libraries(imgui_sfml
            ${CONAN_LIBS}
            )
endif()