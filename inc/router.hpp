#pragma once
#include <memory>
#include <unordered_set>
#include "layout.hpp"
#include "grid.hpp"
class Router
{
public:
    Layout *layout;
    Grid *grid;
    Grid *pin_and_obstacle_grid;
    static const std::vector<std::vector<Coordinate3D>> move_orientation;
    double history_cost = 1; // history cost weight
    double overlapped_cost = 5; // overlapped cost weight
    size_t num_of_reroute; // for counting rerouting number
    size_t num_of_violation; // for counting rerouting number
    Router() {}
    Router(Layout *l) {
        this->layout = l;
        this->grid = new Grid(l->width, l->height, l->num_of_layers);
        this->pin_and_obstacle_grid = new Grid(this->layout);
        this->num_of_reroute = 0;
        this->num_of_violation = 0;
        // number of net + 1 means obstacle
        for(auto o : l->obstacles) this->grid->setObstacles(l->netlist.size(), o.start_point, o.end_point);
        for(auto n : l->netlist){
            for(auto p : n.pins) this->grid->setObstacles(n.id, p, p);
        }
    }
    ~Router() {
        delete this->grid;
        delete this->pin_and_obstacle_grid;
    }

    void main();
    void initial_routing();
    bool outOfBound(Coordinate3D p);
    // bool tree2treeMazeRouting(Net *net, Subtree *source, Subtree *sink);
    Path tree2treeMazeRouting(Grid *current_grid, Net *net, Subtree *source);
    void addHistoryCost(Path *p);
    void insertPathsToGrid(Grid *grid, Path *insert_candidate);
    bool splitPaths(Grid *grid, Coordinate3D point, Path *split_candidate, std::vector<Path*> &updated_paths);
    // counting
    void countNumOfViolation(){
        std::unordered_set<Path*> count_violation;
        for(auto x : grid->graph){
            for(auto y : x){
                for(auto z : y){
                    if(z->cur_paths.size() > 1){
                        for(auto e : z->cur_paths){
                            count_violation.insert(e);
                        }
                    }
                }
            }
        }
        num_of_violation = count_violation.size();
    }
};

// std::pair<int, int> ripUpPaths(Grid *grid, Path *rip_up_candidate, Tree *updated_tree);
// void mergePaths();

