#include <algorithm>
#include <iostream>
#include <tuple>
#include <deque>
#include "router.hpp"
void Router::initial_routing() {
    for(auto &n : layout->netlist){
        n.initTrees();
        std::cout << "Routing Net#" << n.id << "\n";
        while(1){
            int min_root = n.isConnect();
            if (min_root == -1) break;

            Path *path = new Path(tree2treeMazeRouting(grid, &n, n.tree->at(min_root)));

            int source_index = n.tree->findSubtree(path->start_pin);
            int sink_index = n.tree->findSubtree(path->end_pin);

            // Check if both source_index and sink_index are not equal to min_root
            if (source_index != min_root && sink_index != min_root) {
                throw std::runtime_error("Both source_index and sink_index are not equal to min_root");
            }
            n.tree->at(min_root)->paths.push_back(path);
            n.tree->mergeTree(source_index, sink_index);
            // Check the path at path's start_pin and end_pin whether need to be splited
            auto sp_on_grid = grid->graph.at(path->start_pin.x).at(path->start_pin.y).at(path->start_pin.z);
            auto ep_on_grid = grid->graph.at(path->end_pin.x).at(path->end_pin.y).at(path->end_pin.z);
            size_t sp_counter = 0, ep_counter = 0;
            for(auto i : sp_on_grid->cur_paths){
                if(i->id == path->id) sp_counter++;
            }
            for(auto i : ep_on_grid->cur_paths){
                if(i->id == path->id) ep_counter++;
            }
            if(sp_counter == 1){
                splitPaths(grid, path->start_pin, path, n.tree->at(source_index)->paths);
                std::cout << "sp_counter: " << sp_counter << "\n";
            }
            if(ep_counter == 1){
                splitPaths(grid, path->end_pin, path, n.tree->at(sink_index)->paths);
                std::cout << "ep_counter: " << ep_counter << "\n";
            }
            insertPathsToGrid(grid, path);
        }
    }
}
void Router::main(){
    initial_routing();
}