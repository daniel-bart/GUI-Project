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


//Input Formular string from user
std::string user_input_EA;
std::string user_input_TG;
std::string residue_formular;
//Mass and count of Formular
double complete_mass_EA{0.0};
double complete_mass_TG{0.0};
double mass_TG_residue{0.0};
double complete_count_EA{0.0};
//Array to save percentages
std::array<double,5> mass_percentages;
std::array<double,5> atom_percentages;


/**
 * @brief Structure wich initializes/constructs with all masses as map and initializing a counting map
 * 
 */
struct Data
{
    std::map<std::string, std::uint32_t> EA_count;
    std::map<std::string, std::uint32_t> TG_count;
    std::map<std::string, std::uint32_t> TG_residue_count;
    std::map<std::string, double> mass;
    const std::vector<std::string> false_friends {"Cl", "Ca", "Cr", "Co", "Cu", "Cd", "Cs", "Ce", "He", "Ho",
    "Hf", "Hg", "Ne", "Na", "Ni", "Nb", "Nd", "Si", "Sc", "Se", "Sr", "Sn", "Sb", "Sm", "Ba", "Bi", "Be",
    "Br", "Pd", "Pr", "Pm", "Pt", "Pb", "Po", "Fe", "Fr", "Kr", "In", "Ir", "Yb", "Os"};
    //Estimated Oxidation States of non volatile elements after TG, Chalcogenes in dependence of metall
    std::map<std::string, std::uint32_t> non_volatile_elements {{"Li", +1}, {"Be", +2}, {"Na", +1}, {"Mg", +2}, {"Al", +3}, {"Si", +4}, {"K", +1}, {"Ca", +2}, {"Sc", +3}, {"Ti", +4}, {"V", +5}, {"Cr", +3},
    {"Mn", +4}, {"Fe", +3}, {"Co", +2}, {"Ni", +2}, {"Cu", +1}, {"Zn", +2}, {"Ga", +3}, {"Ge", +4}, {"Rb", +1}, {"Sr", +2}, {"Y", +3},{"Zr", +4}, {"Nb", +5}, {"Mo", +6},{"Ru", +4},
    {"Rh", 0}, {"Pd", 0}, {"Ag", 0}, {"Cd", +2}, {"In", +3}, {"Sn", +4}, {"Sb", +4}, {"Cs", +1}, {"Ba", +2}, {"La", +3}, {"Ce", +4}, {"Pr", +3}, {"Nd", +3}, {"Sm", +3}, {"Eu", +3},{"Gd", +3},
    {"Tb", +3},{"Dy", +3},{"Ho", +3},{"Er", +3},{"Tm", +3},{"Yb", +3},{"Lu", +3},{"Hf", +4}, {"Ta", +5}, {"W", +6}, {"Re", +7}, {"Os", +0},{"Ir", +4}, {"Pt", 0}, {"Au", 0}, {"Hg", 0}, {"Tl", +1},
    {"Pb", +2}, {"Bi", +3}, {"P", +5}, {"S", +6}, {"As", +5}, {"Se", +6}, {"Te", +6}};
    const std::vector<std::string> volatile_elements {"H", "He", "N", "C", "O", "Ne", "Ar", "Kr", "Xe"};
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
        Data::EA_count[element_symbols[i]] = 0;
        Data::TG_count[element_symbols[i]] = 0;
        Data::TG_residue_count[element_symbols[i]] = 0;
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
    std::size_t element_found{formular.find(element)};
    std::uint32_t temp_count{0};
    std::size_t element_length{element.length()};
    std::size_t element_count_pos = element_found + element_length; //count begins after found pos + length of element name
    while(element_found != std::string::npos){
        //Check if count_pos is out of string range
        if(element_count_pos >= formular.length()){
        temp_count += 1;
        break;
        }
        else{
        //distinguish if elementcout is over 1/10/100 or if it's two following letters
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
                //Check if Element symbol + string at counting position equals a false friend like (C + e -> Ce) and then skip the counting for that hit
                if(element_length == 1 and std::find(Elements.false_friends.begin(), Elements.false_friends.end(), element + formular.at(element_count_pos)) != Elements.false_friends.end()){
                //std::cout << "False Friend detected!" << std::endl;
                //Skip counting
                }
                else{
                temp_count += 1;
                }
                }
            element_found = formular.find(element, element_count_pos);
            element_count_pos = element_found + element_length;
        }
    }
        return temp_count;
}

/**
 * @brief Calculates the masspercentage of elements
 *
 * @param
 * @return fills array with corresponding percentages
 */
void mass_percentages_EA(){
    if(complete_mass_EA > 0.0){
        //Dont use pointers (const char * arrays) as keys!! new char array equals new key (map uses pointer for comparison, not the string)
        mass_percentages[0]= (( Elements.EA_count["C"]* Elements.mass["C"]) / complete_mass_EA) *100;
        mass_percentages[1]= (( Elements.EA_count["H"]* Elements.mass["H"]) / complete_mass_EA) *100;
        mass_percentages[2]= (( Elements.EA_count["N"]* Elements.mass["N"]) / complete_mass_EA) *100;
        mass_percentages[3]= (( Elements.EA_count["S"]* Elements.mass["S"]) / complete_mass_EA) *100;
        mass_percentages[4]= 100.0-mass_percentages[0]-mass_percentages[1]-mass_percentages[2]-mass_percentages[3];
        atom_percentages[0]=Elements.EA_count["C"] / complete_count_EA*100;
        atom_percentages[1]=Elements.EA_count["H"] / complete_count_EA*100;
        atom_percentages[2]=Elements.EA_count["N"] / complete_count_EA*100;
        atom_percentages[3]=Elements.EA_count["S"] / complete_count_EA*100;
        atom_percentages[4]=100-atom_percentages[0]-atom_percentages[1]-atom_percentages[2]-atom_percentages[3];

        if(atom_percentages[4] < 0 or mass_percentages[4] < 0)
        {
            atom_percentages[4] = 0.0;
            mass_percentages[4] = 0.0;
        }
    }
    else{
        for(auto &val : mass_percentages){
            val = 0.0;
        }
        for(auto &val : atom_percentages){
            val = 0.0;
        }
    }
}

//Search for inorganic elements, that do not leave compound when burned: All elements except: H, He, Ne, Ar, Kr, Xe, Rn, C, N, O,
//Special Exception: F, Cl, Br, I (forming Salts), Sulfur (SO2, Sulfates with Metalcations), Phosphor (P4O10 or Phophates), Se (SeO2 or Selenates), same for Sb and Te
//let user correct residue formular
std::string get_oxide(const std::string &element){
    std::string temp_form;
    switch(Elements.non_volatile_elements[element])
    {
        case 0:
        return element;
        case +1:
        return element + std::to_string(Elements.TG_count[element]*2)+ "O"+std::to_string(Elements.TG_count[element]);
        case +2:
        return element + std::to_string(Elements.TG_count[element])+ "O"+std::to_string(Elements.TG_count[element]);
        case +3:
        return element + std::to_string(Elements.TG_count[element]*2)+ "O"+std::to_string(Elements.TG_count[element]*3);
        case +4:
        return element + std::to_string(Elements.TG_count[element])+ "O"+std::to_string(Elements.TG_count[element]*2);
        case +5:
        return element + std::to_string(Elements.TG_count[element]*2)+ "O"+std::to_string(Elements.TG_count[element]*5);
        case +6:
        return element + std::to_string(Elements.TG_count[element])+ "O"+std::to_string(Elements.TG_count[element]*3);
        case +7:
        return element + std::to_string(Elements.TG_count[element]*2)+ "O"+std::to_string(Elements.TG_count[element]*7);

        default:
        return element + std::to_string(Elements.TG_count[element]);
    }
}
void estimate_residue_formular(std::string &estimate){
    estimate = "";
    for(auto const &pair : Elements.TG_count){
        if(pair.second > 0){
            //Check, that element is not in list of volatile elements, K is hardcoded cause of Kr
            //ToDo: Handle Halogenides as well as chalcogenes
            if(std::find(Elements.volatile_elements.begin(), Elements.volatile_elements.end(), pair.first) == Elements.volatile_elements.end() or pair.first == "K"){
                estimate += get_oxide(pair.first);
        }
        }


    }
}
/**
 * @brief Convert String input into chemical composition and calc EA
 *
 * @param input
 */
void split_into_elements(std::string &input, const std::string &use_case){

    //Differentiate between EA and TG
    if(use_case == "EA"){
    complete_mass_EA = 0.0;
    //iterating over elements, searching for string hits and adding their counts
    //also calc the complete mass of the given formular
    for(auto &pair : Elements.EA_count){
        pair.second = count_of_element(pair.first, input);
        if(pair.second > 0){
            complete_count_EA += pair.second;
            complete_mass_EA += pair.second * Elements.mass[pair.first];
        }
    }
    //Order user input for better readability
    input ="";
    for(auto const &pair: Elements.EA_count){
        if(pair.second > 0){
            input += pair.first + std::to_string(pair.second);
        }
    }

    //calculating the percentages for the EA Elements.
    mass_percentages_EA();
    }


    else if (use_case == "TG")
    {
        complete_mass_TG = 0.0;
        for(auto &pair : Elements.TG_count){
        pair.second = count_of_element(pair.first, input);
        if(pair.second > 0){
            complete_mass_TG += pair.second * Elements.mass[pair.first];
        }

    }
    estimate_residue_formular(residue_formular);
    }
    else if(use_case == "residue"){
        mass_TG_residue = 0.0;
        for(auto &pair : Elements.TG_residue_count){
        pair.second = count_of_element(pair.first, input);
        if(pair.second > 0){
            mass_TG_residue += pair.second * Elements.mass[pair.first];
        }  }
    }
}

void render()
{
    ImGui::NewFrame();
    ImGui::Begin("EA");
    {
        ImGui::InputText("Formula", &user_input_EA);
        if(ImGui::Button("Submit")){
        split_into_elements(user_input_EA, "EA");
            }
        if(ImGui::BeginTable("Masspercentages of Elements", 6, ImGuiTableFlags_Borders)){
            ImGui::TableNextColumn();
            ImGui::TableNextColumn();
            ImGui::Text("C");
            ImGui::TableNextColumn();
            ImGui::Text("H");
            ImGui::TableNextColumn();
            ImGui::Text("N");
            ImGui::TableNextColumn();
            ImGui::Text("S");
            ImGui::TableNextColumn();
            ImGui::Text("Residue");
            ImGui::TableNextColumn();
            ImGui::Text("m%%");
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
            ImGui::TableNextColumn();
            ImGui::Text("at%%");
             ImGui::TableNextColumn();
            ImGui::Text("%f", atom_percentages[0]);
            ImGui::TableNextColumn();
            ImGui::Text("%f", atom_percentages[1]);
            ImGui::TableNextColumn();
            ImGui::Text("%f", atom_percentages[2]);
            ImGui::TableNextColumn();
            ImGui::Text("%f", atom_percentages[3]);
            ImGui::TableNextColumn();
            ImGui::Text("%f", atom_percentages[4]);
        ImGui::EndTable();
        }
    }
    ImGui::End();

     ImGui::Begin("TG");
    {
        ImGui::InputText("Formula", &user_input_TG);
        if(ImGui::Button("Submit")){
        split_into_elements(user_input_TG, "TG");
        }
        ImGui::InputText("Estimated Residue", &residue_formular);
        if(ImGui::Button("Calc")){
        split_into_elements(residue_formular, "residue");
        }
        if(ImGui::BeginTable("Loss of mass", 1, ImGuiTableFlags_Borders)){
            ImGui::TableNextColumn();
            ImGui::Text("Estimated mass in m%%");
            ImGui::NextColumn();
            ImGui::Text("%f", mass_TG_residue/complete_mass_TG*100);
        ImGui::EndTable();}
    }
    ImGui::End();





    ImGui::Render();
}
