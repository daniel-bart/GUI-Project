#include <iostream> 
#include <algorithm>
#include <vector>
#include <array>
#include "imgui.h"
#include "implot.h"
#include<thread>
#include <misc/cpp/imgui_stdlib.h>
#include <misc/cpp/imgui_stdlib.cpp>
#include "render.hpp"


static const double carbon = 12.011;
static const double hydrogen = 1.0080;
static const double nitrogen = 14.007;
static const double sulfur = 32.06;

//Input Formular sting from user
std::string input;
// Saving element counts in global array C, H, N, S
std::array<std::uint32_t,4> element_counter{0, 0, 0, 0};

/**
 * @brief Searches string for stoichiometry
 * 
 * @param element 
 * @return std::uint32_t 
 */
std::uint32_t distribution_of_element(const char &element, const std::string &formular){
std::size_t element_found{formular.find(element)};
std::uint32_t temp_count{0};
while(element_found != std::string::npos){
        if(std::isdigit(formular[element_found+1])){
            if(std::isdigit(formular[element_found+2])){
                if(std::isdigit(formular[element_found+3])){
                    //Numbers start at ASCII of 48
                    temp_count += (formular[element_found+1]-48)*100 + (formular[element_found+2]-48) * 10 + (formular[element_found+3]-48);
                }
                else{
                    temp_count += (formular[element_found+1]-48)*10 + (formular[element_found+2]-48);
                }
            }
            else{
                temp_count += (formular[element_found+1]-48);
            }
        }
        else{
            temp_count += 1;
            }
    element_found = formular.find(element, element_found+1);
    }
    return temp_count;
}
/**
 * @brief Convert String input into chemical composition
 * 
 * @param input 
 */
void split_into_elements(std::string &input){
    //ToDo: Find Inorganic Elements and handle them for TG calc
    std::transform(input.begin(), input.end(),input.begin(),[](unsigned char x){return std::tolower(x);});

    element_counter[0] = distribution_of_element('c', input);
    element_counter[1] = distribution_of_element('h', input);
    element_counter[2] = distribution_of_element('n', input);
    element_counter[3] = distribution_of_element('s', input);

    input= "C" + std::to_string(element_counter[0]) + "H" + std::to_string(element_counter[1]) + "N" + std::to_string(element_counter[2]) + "S" +std::to_string(element_counter[3]);



}
/**
 * @brief Calculates the masspercantage of the given element
 * 
 * @param value 
 * @return std::string 
 */
double mass_percentage(const char &element){
    double complete_mass = element_counter[0] * carbon + element_counter[1] * hydrogen + element_counter[2] * nitrogen + element_counter[3] * sulfur;
    double temp_percentage{0};
    if(complete_mass > 0){
    switch(element){
        case 'c':
        temp_percentage = ((element_counter[0] * carbon) / complete_mass) *100;
        break;
        case 'h':
        temp_percentage = ((element_counter[1] * hydrogen) / complete_mass) *100;
        break;
        case 'n':
        temp_percentage = ((element_counter[2] * nitrogen) / complete_mass) *100;
        break;
        case 's':
        temp_percentage = ((element_counter[3] * sulfur) / complete_mass) *100;
        break;
    }}
    else{
        return 0.0;
    }
    return temp_percentage;
}





void render()
{   
    ImGui::NewFrame();
    ImGui::Begin("EA");
    ImGui::InputText("Formula", &input); 
    if(ImGui::Button("Submit")){
       split_into_elements(input);
    }
        ImGui::BeginTable("Masspercentages of Elements", 4);
            ImGui::TableNextColumn();
            ImGui::Text("C/m%%");
            ImGui::TableNextColumn();
            ImGui::Text("H/m%%");
            ImGui::TableNextColumn();
            ImGui::Text("N/m%%");
            ImGui::TableNextColumn();
            ImGui::Text("S/m%%");
            ImGui::TableNextColumn();
            ImGui::Text("%f", mass_percentage('c'));
            ImGui::TableNextColumn();
            ImGui::Text("%f", mass_percentage('h'));
            ImGui::TableNextColumn();
            ImGui::Text("%f", mass_percentage('n'));
            ImGui::TableNextColumn();
            ImGui::Text("%f", mass_percentage('s'));
        ImGui::EndTable();
    ImGui::End();
   


    

    ImGui::Render();
}
