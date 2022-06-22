#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <array>
#include "imgui.h"
#include "implot.h"
#include <misc/cpp/imgui_stdlib.h>
#include <misc/cpp/imgui_stdlib.cpp>
#include "render.hpp"


//Input Formular sting from user
std::string user_input_formular;
//Mass of Formular
double complete_mass{0.0};
//Array to save percentages
std::array<double,5> mass_percentages;

//Structure wich initializes/constructs with all masses as map and initializing a counting map
struct Data
{
    std::map<std::string, std::uint32_t> count;
    std::map<std::string, double> mass;
    Data()
    {
    const std::vector<std::string> element_symbols{"H", "He", "Li", "Be", "B", "C", "N", "O", "F", "Ne",
    "Na", "Mg", "Al", "Si", "P", "S", "Cl", "Ar",
    "K", "Ca", "Sc", "Ti", "V", "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn", "Ga", "Ge", "As", "Se", "Br", "Kr",
    "Rb", "Sr", "Y", "Zr", "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn", "Sb", "Te", "I", "Xe",
    "Cs", "Ba", "La", "Ce", "Pr", "Nd", "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb", "Lu", "Hf",
    "Ta", "W", "Re", "Os", "Ir", "Pt", "Au", "Hg", "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra"};
    const std::vector<double> element_masses{1.008,4.002,6.941,9.012,10.811,12.011,14.007,15.999,18.998,20.18,22.99,24.305,26.982,28.086,30.974,32.065,35.453,39.948,
    39.098,40.078,44.956,47.867,50.942,51.996,54.938,55.845,58.933,58.693,63.546,65.38,69.723,72.64,74.922,78.96,79.904,83.798,85.468,87.62,88.906,91.224,92.906,
    95.96,98,101.07,102.906,106.42,107.868,112.411,114.818,118.71,121.76,127.6,126.904,131.293,132.905,137.327,138.905,140.116,140.908,144.242,145,150.36,151.964,157.25,
    158.925,162.5,164.93,167.259,168.934,173.054,174.967,178.49,180.948,183.84,186.207,190.23,192.217,195.084,196.967,200.59,204.383,207.2,208.98,210,210,222,223,226};
    for(std::size_t i = 0; i < element_symbols.size(); ++i){
        Data::count[element_symbols[i]] = 0;
        Data::mass[element_symbols[i]] = element_masses[i];
    }
    }


};

Data Elements;
/**
 * @brief Searches string for stoichiometry
 *
 * @param element
 * @return std::uint32_t
 */
std::uint32_t count_of_element(const std::string &element, const std::string &formular){
    //ToDo find false friends! like Ca and C or Si and S
    std::size_t element_found{formular.find(element)};
    std::uint32_t temp_count{0};
    std::size_t element_length{element.length()};
    std::size_t element_count_pos = element_found + element_length; //count begins after found pos + length of element name
    while(element_found != std::string::npos){
        //distinguish if elementcout is over 1/10/100
            if(std::isdigit(formular[element_count_pos])){
                if(std::isdigit(formular[element_count_pos+1])){
                    if(std::isdigit(formular[element_count_pos+2])){
                        //Numbers start at ASCII of 48
                        temp_count += (formular[element_count_pos]-48)*100 + (formular[element_count_pos+1]-48) * 10 + (formular[element_count_pos+2]-48);
                    }
                    else{
                        temp_count += (formular[element_count_pos]-48)*10 + (formular[element_count_pos+1]-48);
                    }
                }
                else{
                    temp_count += (formular[element_count_pos]-48);
                }
            }
            else{
                temp_count += 1;
                }
            element_found = formular.find(element, element_count_pos);
            element_count_pos = element_found + element_length;
        }
        return temp_count;
}

/**
 * @brief Calculates the masspercentage of elements
 *
 * @param value
 * @return s
 */
void mass_percentage(){
    if(complete_mass > 0.0){
        //Dont use pointers (const char arrays) as keys!! new char array equals new key -.-
        mass_percentages[0]= (( Elements.count["C"]* Elements.mass["C"]) / complete_mass) *100;
        mass_percentages[1]= (( Elements.count["H"]* Elements.mass["H"]) / complete_mass) *100;
        mass_percentages[2]= (( Elements.count["N"]* Elements.mass["N"]) / complete_mass) *100;
        mass_percentages[3]= (( Elements.count["S"]* Elements.mass["S"]) / complete_mass) *100;
        mass_percentages[4]= 100.0-mass_percentages[0]-mass_percentages[1]-mass_percentages[2]-mass_percentages[3];
    }
}


/**
 * @brief Convert String input into chemical composition and calc EA
 *
 * @param input
 */
void split_into_elements(std::string &input){
    complete_mass = 0.0;
    //iterating over elements, searching for string hits and adding their counts
    //also calc the complete mass of the given formular
    for(auto &pair : Elements.count){
        pair.second = count_of_element(pair.first, input);
        if(pair.second > 0){
        complete_mass += pair.second * Elements.mass[pair.first];
        }
    }
    //calculating the percentages for the EA Elements.
    mass_percentage();



}






void render()
{
    ImGui::NewFrame();
    ImGui::Begin("EA");
    ImGui::InputText("Formula", &user_input_formular);
    if(ImGui::Button("Submit")){
       split_into_elements(user_input_formular);
    }
        ImGui::BeginTable("Masspercentages of Elements", 5);
            ImGui::TableNextColumn();
            ImGui::Text("C/m%%");
            ImGui::TableNextColumn();
            ImGui::Text("H/m%%");
            ImGui::TableNextColumn();
            ImGui::Text("N/m%%");
            ImGui::TableNextColumn();
            ImGui::Text("S/m%%");
            ImGui::TableNextColumn();
            ImGui::Text("Residue/m%%");
            ImGui::TableNextColumn();
            ImGui::Text("%f", mass_percentages[0]);
            ImGui::TableNextColumn();
            ImGui::Text("%f", mass_percentages[1]);
            ImGui::TableNextColumn();
            ImGui::Text("%f", mass_percentages[2]);
            ImGui::TableNextColumn();
            ImGui::Text("%f", mass_percentages[3]);
            ImGui::TableNextColumn();
            ImGui::Text("%f", mass_percentages[4]);
        ImGui::EndTable();
    ImGui::End();





    ImGui::Render();
}
