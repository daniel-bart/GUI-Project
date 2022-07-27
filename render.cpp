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
#include <sstream>


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


bool isInteger(double N)
{
    int X = N;

    double temp = N - X;
    if (temp > 0) {
        return false;
    }
    return true;
}
//set precision when converting to string
template<typename T>
std::string to_string_with_precision(T a_value, const int n = 1)
{
    if(isInteger(a_value)){
    std::ostringstream out;
    out.precision(0);
    out << std::fixed << a_value;
    return out.str();
    }
    else{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
    }
}

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
    //Added Halogens and Chalcogene, cause they are volatile if no metal is present
    const std::vector<std::string> volatile_elements {"H", "He", "N", "C", "O", "Ne", "Ar", "Kr", "Xe", "F", "Cl", "Br", "I", "S", "P", "As", "Se", "Te"};
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

std::uint32_t count_char(const std::string &input, const char* p){
    std::size_t hit{input.find(p)};
    std::uint32_t count{0};
    while(hit != std::string::npos)
    {
        count++;
        hit = input.find(p, hit+1);
    }
    return count;
}
std::string check_brackets(const std::string &formular){
    std::string temp_formular{formular};
    std::uint32_t num_brackets_open{count_char(formular, "(")};
    std::uint32_t num_brackets_close{count_char(formular, ")")};
    auto cast_to_string_obj = [](const char &x){int y{x-48}; return to_string_with_precision(y);};
    if(num_brackets_open == num_brackets_close){
        //No Brackets
        if(num_brackets_open == 0){
            return temp_formular;
        }
        // One Bracket
        else if(num_brackets_open == 1){
            std::size_t bracket_begin{temp_formular.find("(")};
            std::size_t bracket_end{temp_formular.find(")")};
                if(isdigit(temp_formular[bracket_end+1])){
                    char temp_multi{temp_formular[bracket_end+1]};
                    for(std::size_t i = bracket_begin+1; i != bracket_end; i++ ){
                        if(std::isdigit(temp_formular[i])){
                            temp_formular[i] = (temp_formular[i]-48)* (temp_multi-48)+48;
                        }
                        else if(!std::isdigit(temp_formular[i+1]) and !std::islower(temp_formular[i+1])){
                            //Todo what if number after last bracket is a 2 digit number?
                            // Also in if statement before, what if 2 digit number? -> See element count alg
                            temp_formular.insert(i+1, cast_to_string_obj(temp_multi));
                            bracket_end += 1;
                            i++;
                        }
                    }
                temp_formular.erase(bracket_begin,1);
                temp_formular.erase(bracket_end,2);
                }
                else{
                    temp_formular.erase(bracket_begin,1);
                    temp_formular.erase(bracket_end,1);
                }

            return temp_formular;
            }
        //Multiple Brackets
        else{
            std::vector<std::size_t> opening_brackets;
            std::vector<std::size_t> opening_brackets_temp;
            std::vector<std::size_t> closing_brackets;
            std::size_t bracket_begin{temp_formular.find("(")};
            std::size_t bracket_end{temp_formular.find(")")};
            //collect positions of pairs of opening and closing brackets in corresponding vec
            while(bracket_begin != std::string::npos){
                opening_brackets_temp.push_back(bracket_begin);
                bracket_begin = temp_formular.find("(", bracket_begin+1);
            }
            while(bracket_end != std::string::npos){
                closing_brackets.push_back(bracket_end);
                bracket_end = temp_formular.find(")", bracket_end+1);
            }
            for(std::size_t a = 0; a != num_brackets_close; a++){
                //search opening bracket with position lower than closing bracket and with the smallest diff
                //kind of sorting bracket position so that openings_bracket[n] are matching the closing_brackets[n]
                auto it = std::upper_bound(opening_brackets_temp.begin(), opening_brackets_temp.end(), closing_brackets[a]);
                auto prev = std::prev(it);
                opening_brackets.push_back(*prev);
                opening_brackets_temp.erase(prev);

            }

            for(std::size_t i = 0; i != num_brackets_open; i++){
                if(isdigit(temp_formular[closing_brackets[i]+1])){
                    char temp_multi{temp_formular[closing_brackets[i]+1]};
                    for(std::size_t j = opening_brackets[i]+1; j != closing_brackets[i]; j++ ){
                        //Todo adjust for higher numbers
                        if(std::isdigit(temp_formular[j])){
                            temp_formular[j] = (temp_formular[j]-48)* (temp_multi-48)+48;
                        }
                        //Todo adjust for higher numbers
                        else if(!std::isdigit(temp_formular[j+1]) and temp_formular[j] != ')' and temp_formular[j] != '(' and !std::islower(temp_formular[j+1])){
                            temp_formular.insert(j+1, cast_to_string_obj(temp_multi));
                            //positions of brackets following the insert(position >= j+1) have to be increased by one!
                            for(auto &range : opening_brackets){
                                if(range >= j+1){
                                    range++;
                                }
                            }
                            for(auto &range : closing_brackets){
                                if(range >= j+1){
                                    range++;
                                }
                            }
                            //Todo adjust for higher numbers
                            j++; //skip inserted number, carefull im 2 digit number!
                        }
                    }
                    //Todo adjust for higher numbers
                temp_formular.erase(opening_brackets[i],1);
                temp_formular.erase(closing_brackets[i],2);
                std::size_t temp_open_bracket_pos{opening_brackets[i]};
                std::size_t temp_close_bracket_pos{closing_brackets[i]};
                //ranges have to be adjusted again for both deletions
                for(auto &range : opening_brackets){
                                if(range > temp_open_bracket_pos and range > temp_close_bracket_pos){
                                    range-=3;
                                }
                                else if(range > temp_open_bracket_pos and range < temp_close_bracket_pos){
                                    range--;
                                }
                            }
                for(auto &range : closing_brackets){
                                if(range > temp_open_bracket_pos and range > temp_close_bracket_pos){
                                    range-= 3;
                                }
                                else if(range > temp_open_bracket_pos and range < temp_close_bracket_pos){
                                    range--;
                                }
                            }

                }
                else{
                temp_formular.erase(opening_brackets[i],1);
                temp_formular.erase(closing_brackets[i],1);
                //ranges have to be adjusted again but without the deletion of a number behind the closing bracket
                std::size_t temp_open_bracket_pos{opening_brackets[i]};
                std::size_t temp_close_bracket_pos{closing_brackets[i]};

                for(auto &range : opening_brackets){
                                if(range > temp_open_bracket_pos and range > temp_close_bracket_pos){
                                    range-=2;
                                }
                                else if(range > temp_open_bracket_pos and range < temp_close_bracket_pos){
                                    range--;
                                }
                            }
                for(auto &range : closing_brackets){
                                if(range > temp_open_bracket_pos and range > temp_close_bracket_pos){
                                    range-= 2;
                                }
                                else if(range > temp_open_bracket_pos and range < temp_close_bracket_pos){
                                    range--;
                                }
                            }
                }
            }
            return temp_formular;

        }
    }
    else {
        //ToDo wrong brackets
        return "Error";
    }
}
/**
 * @brief Searches string for stoichiometry
 *
 * @param element
 * @return std::uint32_t
 */
std::uint32_t count_of_element(const std::string &element, const std::string &formular){
    //std::string formular{check_brackets(formular_org)};
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
//Special Exception: F, Cl, Br, I (forming Salts), Sulfur (SO2, Sulfates with Metalcations), Phosphor (P4O10 or Phophates), Se (SeO2 or Selenates), same for Te
//But if no Metal is present those elements will be volatile
//let user correct residue formular
std::string get_halchal(const std::string &element, std::map<std::string, std::uint32_t> &map){
    std::string temp_form{element};
    std::uint32_t ox_num = Elements.non_volatile_elements[element];
    std::map<std::string, std::uint32_t> hals {{"F", 0}, {"Cl", 0}, {"Br", 0}, {"I", 0},{"S",0}, {"P",0},{"Se",0}, {"Te",0}, {"O",0}};

            //for P,S,Se,Te i has to be further incremented, cause of their charge (eg. PO4 3-)
            for(std::uint32_t i = 0; i != ox_num; i++){
                if(map["F"] > 0){
                hals["F"]++;
                map["F"]--;
                }
                else if(map["Cl"] > 0){
                hals["Cl"]++;
                map["Cl"]--;
                }
                else if(map["Br"] > 0){
                hals["Br"]++;
                map["Br"]--;
                }
                else if(map["I"] > 0){
                hals["I"]++;
                map["I"]--;
                }
                else if(map["S"] > 0 and (ox_num-i) >= 2){
                hals["S"]++;
                map["S"]--;
                i++;
                }
                else if(map["P"]> 0 and (ox_num-i) >= 3){
                hals["P"]++;
                map["P"]--;
                i+=2;
                }
                else if(map["Se"] > 0 and (ox_num-i) >= 2){
                hals["Se"]++;
                map["Se"]--;
                i++;
                }
                else if(map["Te"] > 0 and (ox_num-i) >= 2){
                hals["Te"]++;
                map["Te"]--;
                i++;
                }
                else if((ox_num-i) >= 2){
                hals["O"]++;
                i++;
                }
            }
            for(auto const &val : hals){
                if(val.second > 1){
                    if(val.first == "S"){
                        temp_form += "(SO4)" + to_string_with_precision(val.second);
                    }
                    else if(val.first == "P"){
                        temp_form += "(PO4)" + to_string_with_precision(val.second);
                    }
                    else if(val.first == "Se"){
                        temp_form += "(SeO4)" + to_string_with_precision(val.second);
                    }
                    else if( val.first == "Te"){
                        temp_form += "(TeO4)" + to_string_with_precision(val.second);
                    }
                    else{
                        temp_form += val.first + to_string_with_precision(val.second);
                    }}
                else if(val.second > 0){
                    if(val.first == "S"){
                        temp_form += "(SO4)";
                    }
                    else if(val.first == "P"){
                        temp_form += "(PO4)";
                    }
                    else if(val.first == "Se"){
                        temp_form += "(SeO4)";
                    }
                    else if( val.first == "Te"){
                        temp_form += "(TeO4)";
                    }
                    else{
                        temp_form += val.first;
                    }
            }
            }

    return temp_form;
}
std::string get_oxide(const std::string &element){
    std::string temp_form;
    auto count_alg = [](const std::uint32_t &val)->std::string{if(val > 1){return std::to_string(val);} else{return "";}};
        if(Elements.non_volatile_elements[element] == 0){
        return element;
        }
        else{
        return element + count_alg(Elements.TG_count[element])+ "O"+to_string_with_precision(Elements.TG_count[element]*Elements.non_volatile_elements[element]/2.0);
        }
}
void estimate_residue_formular(std::string &estimate){
    estimate = "";
    //tempory map for matching metals with halogens and chalcogenes
    std::map<std::string, std::uint32_t> temp_elements{{"F", Elements.TG_count["F"]}, {"Cl", Elements.TG_count["Cl"]}, {"Br", Elements.TG_count["Br"]}, {"I", Elements.TG_count["I"]}, {"S", Elements.TG_count["S"]}, {"P", Elements.TG_count["P"]}, {"Se", Elements.TG_count["Se"]}, {"Te", Elements.TG_count["Te"]}, {"As", Elements.TG_count["As"]}};
    auto halchal_present = [&temp_elements](){for(auto const &val: temp_elements){if(val.second > 0){return true;}}return false;};
    for(auto const &pair : Elements.TG_count){
        if(pair.second > 0){
            std::uint32_t i{0};
            //Check, that element is not in list of volatile elements, K is hardcoded cause of Kr false hits, also B.
            //short: Get non volatile elements (most of them are metals)
            if(std::find(Elements.volatile_elements.begin(), Elements.volatile_elements.end(), pair.first) == Elements.volatile_elements.end() | pair.first == "K" |pair.first == "B"){
                //check if there are halogens or chalcogens present
                while(i != pair.second){
                    if(halchal_present()){
                        //get halogenids and chalcogenids stöchiometrie
                        estimate += get_halchal(pair.first, temp_elements);
                        i++;
                    }
                    else{
                        //if no hal and chalcs present, get oxides of elements
                        estimate += get_oxide(pair.first);
                        i = pair.second;
                    }
                }
        }
        }


    }
}
/**
 * @brief Convert String input into chemical composition and calc EA
 *
 * @param input
 */
void split_into_elements(std::string &formular, const std::string &use_case){

    auto count_alg = [](const std::uint32_t &val)->std::string{if(val > 1){return std::to_string(val);} else{return "";}};


    //Differentiate between EA and TG
    //may change to switch syntax later
    if(use_case == "EA"){
    //check user input for brackets
    std::string input{check_brackets(formular)};
    complete_mass_EA = 0.0;
    complete_count_EA = 0;
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
            input += pair.first + count_alg(pair.second);
        }
    }
    //formular = input;

    //calculating the percentages for the EA Elements.
    mass_percentages_EA();
    }


    else if (use_case == "TG")
    {
        //check user input for brackets
        std::string input{check_brackets(formular)};
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
        //check user input for brackets
        std::string input{check_brackets(formular)};
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
            ImGui::Text("%.2f", mass_percentages[0]);
            ImGui::TableNextColumn();
            ImGui::Text("%.2f", mass_percentages[1]);
            ImGui::TableNextColumn();
            ImGui::Text("%.2f", mass_percentages[2]);
            ImGui::TableNextColumn();
            ImGui::Text("%.2f", mass_percentages[3]);
            ImGui::TableNextColumn();
            ImGui::Text("%.2f", mass_percentages[4]);
            ImGui::TableNextColumn();
            ImGui::Text("at%%");
             ImGui::TableNextColumn();
            ImGui::Text("%.2f", atom_percentages[0]);
            ImGui::TableNextColumn();
            ImGui::Text("%.2f", atom_percentages[1]);
            ImGui::TableNextColumn();
            ImGui::Text("%.2f", atom_percentages[2]);
            ImGui::TableNextColumn();
            ImGui::Text("%.2f", atom_percentages[3]);
            ImGui::TableNextColumn();
            ImGui::Text("%.2f", atom_percentages[4]);
        ImGui::EndTable();
        }
    }
    ImGui::End();

     ImGui::Begin("TG");
    {
        ImGui::InputText("Formula", &user_input_TG);
        if(ImGui::Button("Estimate")){
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
            ImGui::Text("%.2f", mass_TG_residue/complete_mass_TG*100);
        ImGui::EndTable();}
    }
    ImGui::End();





    ImGui::Render();
}
