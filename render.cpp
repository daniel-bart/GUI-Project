#include <iostream> 
#include "imgui.h"
#include "implot.h"
#include <misc/cpp/imgui_stdlib.h>
#include <misc/cpp/imgui_stdlib.cpp>



#include "render.hpp"


void render()
{   std::string formular;
    ImGui::NewFrame();
    ImGui::Begin("Test Window");
    ImGui::InputText("Formular", &formular); 
    ImGui::End();

    

    ImGui::Render();
}
