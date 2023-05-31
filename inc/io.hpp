#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "layout.hpp"

std::stringstream ss;

namespace io{

void tokenLine(std::vector<std::string> &tokens, std::string line){
    ss.clear(); ss.str(line);
    tokens.clear(); tokens.resize(0);
    std::string intermediate;
    while(true){
        ss >> intermediate;
        if(ss.fail()) break;
        tokens.push_back(intermediate);
    }
}

void readLayout(Layout *layout, char const *file_path){
    std::ifstream in_file(file_path);
    std::string line;
    std::vector<std::string> tokens;
    while(getline(in_file, line)){
        io::tokenLine(tokens, line);
        if(tokens.size() == static_cast<unsigned int>(2)){
            if(tokens[0] == "Width"){
                layout->width = stoi(tokens[1]);
            }
            else if(tokens[0] == "Height"){
                layout->height = stoi(tokens[1]);
            }
            else if(tokens[0] == "Layer"){
                layout->num_of_layers = stoi(tokens[1]);
            }
            else if(tokens[0] == "Obstacle_num"){
                layout->obstacles.resize(stoi(tokens[1]));
                for(unsigned i = 0; i < layout->obstacles.size(); i++){
                    getline(in_file, line); io::tokenLine(tokens, line);
                    if(stoi(tokens[2]) == 0)
                        layout->obstacles.at(i) = Obstacle{stoi(tokens[0]), stoi(tokens[1]), stoi(tokens[2])
                                        , stoi(tokens[3]) - 1, stoi(tokens[4]) - 1, stoi(tokens[5])};
                    else
                        layout->obstacles.at(i) = Obstacle{stoi(tokens[0]), stoi(tokens[1]), stoi(tokens[2])
                                        , stoi(tokens[3]) - 1, stoi(tokens[4]) - 1, stoi(tokens[5])};
                }
            }
            else if(tokens[0] == "Net_num"){
                layout->netlist.resize(stoi(tokens[1]));
                for(unsigned i = 0; i < layout->netlist.size(); i++){
                    Net &tmp_net = layout->netlist.at(i);
                    while(getline(in_file, line)){
                        io::tokenLine(tokens, line);
                        if(tokens.size() == static_cast<unsigned int>(2) && tokens[0] == "Net_id") break;
                    }
                    tmp_net.id = stoi(tokens[1]);
                    while(getline(in_file, line)){
                        io::tokenLine(tokens, line);
                        if(tokens.size() == static_cast<unsigned int>(2) && tokens[0] == "pin_num") break;
                    }
                    tmp_net.pins.resize(stoi(tokens[1]));
                    for(unsigned j = 0; j < tmp_net.pins.size(); j++){
                        getline(in_file, line); io::tokenLine(tokens, line);
                        tmp_net.pins.at(j) = Coordinate3D{stoi(tokens[0]), stoi(tokens[1]), stoi(tokens[2])};
                    }
                }
            }
            else if(tokens[0] == "Via_cost"){
                layout->via_cost = stoi(tokens[1]);
            }
            else if(tokens[0] == "Horizontal_segment_cost"){
                layout->horizontal_segment_cost = stoi(tokens[1]);
            }
            else if(tokens[0] == "Vertical_segment_cost"){
                layout->vertical_segment_cost = stoi(tokens[1]);
            }
        }
    }
}

void writeLayout(Layout *layout, char const *file_path){
    /* Initial */
    for(unsigned i = 0; i < layout->netlist.size(); i++) {
        layout->netlist.at(i).segmentRegularize();
    }
    std::ofstream out_file(file_path, std::ofstream::trunc);
    out_file << "Width " << layout->width << "\n";
    out_file << "Height " << layout->height << "\n";
    out_file << "Layer " << layout->num_of_layers << "\n";
    out_file << "Total_WL " << layout->getWirelength() << "\n";
    out_file << "Cost " << layout->getCost() << "\n";
    out_file << "Obstacle_num " << layout->obstacles.size() << "\n";
    for(unsigned i = 0; i < layout->obstacles.size(); i++){
        out_file << layout->obstacles.at(i).start_point.x << " " << layout->obstacles.at(i).start_point.y << " " << layout->obstacles.at(i).start_point.z << " ";
        if(layout->obstacles.at(i).start_point.z == 0)
            out_file << layout->obstacles.at(i).end_point.x + 1 << " " << layout->obstacles.at(i).end_point.y + 1 << " " << layout->obstacles.at(i).end_point.z;
        else
            out_file << layout->obstacles.at(i).end_point.x + 1 << " " << layout->obstacles.at(i).end_point.y + 1 << " " << layout->obstacles.at(i).end_point.z;
        out_file << "\n";
    }
    out_file << "Net_num " << layout->netlist.size() << "\n";
    for(unsigned i = 0; i < layout->netlist.size(); i++){
        out_file << "Net_id " << layout->netlist.at(i).id << "\n";
        out_file << "pin_num " << layout->netlist.at(i).pins.size() << "\n";
        for(unsigned j = 0; j < layout->netlist.at(i).pins.size(); j++){
            out_file << layout->netlist.at(i).pins.at(j).x << " " << layout->netlist.at(i).pins.at(j).y << " " << layout->netlist.at(i).pins.at(j).z << "\n";
        }
        out_file << "Via_num " << layout->netlist.at(i).vialist.size() << "\n";
        for(auto &v : layout->netlist.at(i).vialist){
            out_file << v.x << " " << v.y << " " << v.z1 << " " << v.z2 << "\n";
        }
        out_file << "H_segment_num " << layout->netlist.at(i).horizontal_segments.size() << "\n";
        for(unsigned j = 0; j < layout->netlist.at(i).horizontal_segments.size(); j++){
            out_file << layout->netlist.at(i).horizontal_segments.at(j).start_point.x << " " << layout->netlist.at(i).horizontal_segments.at(j).start_point.y 
                    << " " << layout->netlist.at(i).horizontal_segments.at(j).start_point.z << " ";
            out_file << layout->netlist.at(i).horizontal_segments.at(j).end_point.x + 1 << " " << layout->netlist.at(i).horizontal_segments.at(j).end_point.y + 1
                    << " " << layout->netlist.at(i).horizontal_segments.at(j).end_point.z << "\n";
        }
        out_file << "V_segment_num " << layout->netlist.at(i).vertical_segments.size() << "\n";
        for(unsigned j = 0; j < layout->netlist.at(i).vertical_segments.size(); j++){
            out_file << layout->netlist.at(i).vertical_segments.at(j).start_point.x << " " << layout->netlist.at(i).vertical_segments.at(j).start_point.y 
                    << " " << layout->netlist.at(i).vertical_segments.at(j).start_point.z << " ";
            out_file << layout->netlist.at(i).vertical_segments.at(j).end_point.x + 1 << " " << layout->netlist.at(i).vertical_segments.at(j).end_point.y + 1
                    << " " << layout->netlist.at(i).vertical_segments.at(j).end_point.z << "\n";
        }
    }
    // Set the obstacle_id to Mex of Net id
    // Then find the Mex. Start from 0 and go up until we find a number not in the set.
    layout->obstacle_id = 0;
    std::set<int> ids;
    for(unsigned i = 0; i < layout->netlist.size(); i++){
        ids.insert(layout->netlist.at(i).id);
    }
    while(ids.find(layout->obstacle_id) != ids.end()){
        layout->obstacle_id++;
    }
}
} // namespace io

