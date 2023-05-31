#include <queue>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <type_traits>
#include "router.hpp"
const std::vector<std::vector<Coordinate3D>> Router::move_orientation = {{Coordinate3D(1,0,0), Coordinate3D(-1,0,0), Coordinate3D(0,0,1), Coordinate3D(0,0,-1)},
                            {Coordinate3D(0,1,0), Coordinate3D(0,-1,0), Coordinate3D(0,0,1), Coordinate3D(0,0,-1)}};
double mazeRouteCost(Router *R, Grid *current_grid, Coordinate3D sp, Coordinate3D ep){
    double cost = 0;
    if(sp.z != ep.z){
        cost += R->layout->via_cost;
    }
    cost += std::abs(sp.x - ep.x) * R->layout->horizontal_segment_cost + std::abs(sp.y - ep.y) * R->layout->vertical_segment_cost;
    cost += R->grid->history.at(ep.x).at(ep.y).at(ep.z);
    if(current_grid->graph.at(ep.x).at(ep.y).at(ep.z)->cur_paths.size() > 0) cost += R->overlapped_cost;
    return cost;
}
void Router::addHistoryCost(Path *p){
    for(auto s : p->segments){
        if(s->getLayer() == 0){
            for(int i = s->getX(); i <= s->getNeighbor(); i++){
                grid->history.at(i).at(s->getY()).at(s->getZ()) += history_cost;
            }
        }
        else{
            for(int i = s->getY(); i <= s->getNeighbor(); i++){
                grid->history.at(s->getX()).at(i).at(s->getZ()) += history_cost;
            }
        }
    }
}
/* Insert the path from the grid */
void Router::insertPathsToGrid(Grid *grid, Path *insert_candidate){
    bool find = false;
    // Insert an `Path` including all the segments
    for(auto s : insert_candidate->segments){
        if(s->getLayer() == 0){
            for(int i = s->getX(); i <= s->getNeighbor(); i++){
                auto &cur_path = grid->graph.at(i).at(s->getY()).at(s->getZ())->cur_paths;
                find = false;
                for(auto e : cur_path){
                    if(e == insert_candidate) find = true;
                    if(find) break;
                }
                if(!find) cur_path.push_back(insert_candidate);
            }
        }
        else if(s->getLayer() == 1){
            for(int i = s->getY(); i <= s->getNeighbor(); i++){
                auto &cur_path = grid->graph.at(s->getX()).at(i).at(s->getZ())->cur_paths;
                find = false;
                for(auto e : cur_path){
                    if(e == insert_candidate) find = true;
                    if(find) break;
                }
                if(!find) cur_path.push_back(insert_candidate);
            }
        }
        else if(s->getLayer() == 2){
            for(int i = s->getZ(); i <= s->getNeighbor(); i++){
                auto &cur_path = grid->graph.at(s->getX()).at(s->getY()).at(i)->cur_paths;
                find = false;
                for(auto e : cur_path){
                    if(e == insert_candidate) find = true;
                    if(find) break;
                }
                if(!find) cur_path.push_back(insert_candidate);
            }
        }
    }
}
/* Remove the path from the grid */
void removePathsFromGrid(Grid *grid, Path *remove_candidate){
    // Remove an `Path` including all the segments and the start_pin and end_pin location
    for(auto s : remove_candidate->segments){
        if(s->getLayer() == 0){
            for(int i = s->getX(); i <= s->getNeighbor(); i++){
                auto &cur_path = grid->graph.at(i).at(s->getY()).at(s->getZ())->cur_paths;
                cur_path.erase(std::remove(cur_path.begin(), cur_path.end(), remove_candidate), cur_path.end());
                if(cur_path.size() == 0) grid->resetObstacles(Coordinate3D{i, s->getY(), s->getZ()});
            }
        }
        else if(s->getLayer() == 1){
            for(int i = s->getY(); i <= s->getNeighbor(); i++){
                auto &cur_path = grid->graph.at(s->getX()).at(i).at(s->getZ())->cur_paths;
                cur_path.erase(std::remove(cur_path.begin(), cur_path.end(), remove_candidate), cur_path.end());
                if(cur_path.size() == 0) grid->resetObstacles(Coordinate3D{s->getX(), i, s->getZ()});
            }
        }
        else if(s->getLayer() == 2){
            for(int i = s->getZ(); i <= s->getNeighbor(); i++){
                auto &cur_path = grid->graph.at(s->getX()).at(s->getY()).at(i)->cur_paths;
                cur_path.erase(std::remove(cur_path.begin(), cur_path.end(), remove_candidate), cur_path.end());
                if(cur_path.size() == 0) grid->resetObstacles(Coordinate3D{s->getX(), s->getY(), i});
            }
        }
    }
    // Remove start_pin
    auto sp = remove_candidate->start_pin;
    auto &t = grid->graph.at(sp.x).at(sp.y).at(sp.z)->cur_paths;
    t.erase(std::remove(t.begin(), t.end(), remove_candidate), t.end());
    if(t.size() == 0) grid->resetObstacles(sp);
    // Remove end_pin
    auto ep = remove_candidate->end_pin;
    auto &k = grid->graph.at(ep.x).at(ep.y).at(ep.z)->cur_paths;
    k.erase(std::remove(k.begin(), k.end(), remove_candidate), k.end());
    if(k.size() == 0) grid->resetObstacles(ep);
}
/* Remove the remove_candidate from the grid along the remove_path_locus */
void removePathsFromGrid(Grid *grid, Path *remove_path_locus, Path *remove_candidate){
    // Remove an `Path` including all the segments and the start_pin and end_pin location
    for(auto s : remove_path_locus->segments){
        if(s->getLayer() == 0){
            for(int i = s->getX(); i <= s->getNeighbor(); i++){
                auto &cur_path = grid->graph.at(i).at(s->getY()).at(s->getZ())->cur_paths;
                cur_path.erase(std::remove(cur_path.begin(), cur_path.end(), remove_candidate), cur_path.end());
                if(cur_path.size() == 0) grid->resetObstacles(Coordinate3D{i, s->getY(), s->getZ()});
            }
        }
        else if(s->getLayer() == 1){
            for(int i = s->getY(); i <= s->getNeighbor(); i++){
                auto &cur_path = grid->graph.at(s->getX()).at(i).at(s->getZ())->cur_paths;
                cur_path.erase(std::remove(cur_path.begin(), cur_path.end(), remove_candidate), cur_path.end());
                if(cur_path.size() == 0) grid->resetObstacles(Coordinate3D{s->getX(), i, s->getZ()});
            }
        }
        else if(s->getLayer() == 2){
            for(int i = s->getZ(); i <= s->getNeighbor(); i++){
                auto &cur_path = grid->graph.at(s->getX()).at(s->getY()).at(i)->cur_paths;
                cur_path.erase(std::remove(cur_path.begin(), cur_path.end(), remove_candidate), cur_path.end());
                if(cur_path.size() == 0) grid->resetObstacles(Coordinate3D{s->getX(), s->getY(), i});
            }
        }
    }
    // Remove start_pin
    auto sp = remove_path_locus->start_pin;
    auto &t = grid->graph.at(sp.x).at(sp.y).at(sp.z)->cur_paths;
    t.erase(std::remove(t.begin(), t.end(), remove_candidate), t.end());
    if(t.size() == 0) grid->resetObstacles(sp);
    // Remove end_pin
    auto ep = remove_path_locus->end_pin;
    auto &k = grid->graph.at(ep.x).at(ep.y).at(ep.z)->cur_paths;
    k.erase(std::remove(k.begin(), k.end(), remove_candidate), k.end());
    if(k.size() == 0) grid->resetObstacles(ep);
}

bool Router::outOfBound(Coordinate3D p){
    if(p.x < 0 || p.x >= this->layout->width) return true;
    else if(p.y < 0 || p.y >= this->layout->height) return true;
    else if(p.z < 0 || p.z >= this->layout->num_of_layers) return true;
    else return false;
}

/* When creating a stiener node on a path, the path need to be split*/
bool Router::splitPaths(Grid *grid, Coordinate3D point, Path *split_candidate, std::vector<Path*> &updated_paths){
    split_candidate->lineUpSegments();
    for(unsigned i = 0; i < updated_paths.size(); i++){
        auto &p = updated_paths.at(i);
        if(p == split_candidate){
            Coordinate3D start_point = p->start_pin;
            Path *new_path = new Path();
            bool complete_path = false;;
            new_path->start_pin = p->start_pin;
            std::vector<Segment*> remove_list;
            for(unsigned j = 0; j < p->segments.size(); j++){
                auto &s = p->segments.at(j);
                if(s->colinear(point)){
                    // Split segment
                    if(!(Coordinate2D{point} == Coordinate2D{s->startPoint()} 
                            || Coordinate2D{point} == Coordinate2D{s->endPoint()})){
                        Segment *new_segment = nullptr;
                        if(s->getLayer() == 0){
                            new_segment = new Segment(start_point.x, s->getY(), s->getZ(), 0, point.x);
                            if(s->getNeighbor() == start_point.x) s->setNeighbor(point.x);
                            else s->setX(point.x);
                        }
                        else if(s->getLayer() == 1){
                            new_segment = new Segment(s->getX(), start_point.y, s->getZ(), 1, point.y);
                            if(s->getNeighbor() == start_point.y) s->setNeighbor(point.y);
                            else s->setY(point.y);
                        }
                        else if(s->getLayer() == 2){
                            new_segment = new Segment(s->getX(), s->getY(), start_point.z, 2, point.z);
                            if(s->getNeighbor() == start_point.z) s->setNeighbor(point.z);
                            else s->setZ(point.z);
                        }
                        else{
                            // Handle the case when getLayer() returns an unexpected value
                            throw std::runtime_error("Invalid layer value");
                        }
                        new_path->end_pin = point;
                        p->start_pin = point;
                        if(new_segment != nullptr) new_path->segments.push_back(new_segment);
                        complete_path = true;
                    }
                    // Current segment belongs to old one
                    else if(point == start_point){
                        new_path->end_pin = start_point;
                        p->start_pin = new_path->end_pin;
                        complete_path = true;
                    }
                    // Current segment belongs to  new one, assign to new path
                    else{
                        remove_list.push_back(s);
                        new_path->segments.push_back(s);
                        new_path->end_pin = point;
                        p->start_pin = new_path->end_pin;
                        complete_path = true;
                    }
                    break;
                }
                else{
                    remove_list.push_back(s);
                    new_path->segments.push_back(s);
                    // Move to this segments tail, for find the next segment head
                    start_point = ((Coordinate2D{s->startPoint()} == Coordinate2D{start_point}) ? s->endPoint() : s->startPoint());
                    // Find the next segment head
                    if(j + 1 < p->segments.size()){
                        auto &tmp = p->segments.at(j+1);
                        start_point = (Coordinate2D{tmp->startPoint()} == Coordinate2D{start_point}) ? tmp->startPoint() : tmp->endPoint();
                    }
                    else{
                        throw std::runtime_error("Last segment should not reach here");
                    }
                }
            }
            for (auto it = p->segments.begin(); it != p->segments.end(); ) {
                if (std::find(remove_list.begin(), remove_list.end(), *it) != remove_list.end()) {
                    // Remove the element
                    it = p->segments.erase(it);
                } else {
                    // Move to the next element
                    ++it;
                }
            }
            if(complete_path) {
                insertPathsToGrid(grid, new_path);
                removePathsFromGrid(grid, new_path, p);
                insertPathsToGrid(grid, p);
                updated_paths.push_back(new_path);
                return true;
            }
            else{
                throw std::runtime_error("Error: split path failed have unexpected error");
            }
        }
    }
    return false;
}

/* 
 * Tree2tree maze routing, return the path from source to sink
 * If not success will throw runtime error.
 */
Path Router::tree2treeMazeRouting(Grid *current_grid, Net *net, Subtree *source){
    /* Declaring */
    Vertex *current = nullptr;
    auto comp = [](const Vertex *lhs, const Vertex *rhs) {return lhs->distance > rhs->distance;};
    std::priority_queue<Vertex*, std::vector<Vertex*>, decltype(comp)> pq(comp);
    /* Initialize the soruce and sink verteices */
    // Mark all net pin coordinates as sinks, except for the source pins
    for (const auto& pin : net->pins) {
        if (!source->pinlist.count(pin)) {
            current_grid->setSinks(pin);
        }
    }
    // Mark all segments as sinks, except for the segments belonging to the source pins
    std::set<Path*> tmp_path_set(source->paths.begin(), source->paths.end());
    for (const auto& path : net->tree->getPath()) {
        if(!tmp_path_set.count(path)){
            for(const auto& segment: path->segments) {
                current_grid->setSinks(*segment);
            }
        }
    }
    // Set all vertex's distance to infinity
    current_grid->setDistanceInfinity();
    // Set the source's distance to zero
    grid->setDistanceZero(std::vector<Coordinate3D>(source->pinlist.begin(), source->pinlist.end()));
    for(auto p : source->pinlist){
        pq.push(grid->graph.at(p.x).at(p.y).at(p.z));
    }
    // Push the source into pq
    for(auto p : source->pinlist){
        pq.push(grid->graph.at(p.x).at(p.y).at(p.z));
    }
    for(auto &p : source->paths){
        for(auto &s : p->segments){
            if(s->getLayer() == 0){
                for(int i = std::min(s->getX(), s->getNeighbor()); i <= std::max(s->getX(), s->getNeighbor()); i++){
                    if(this->grid->graph.at(i).at(s->getY()).at(s->getZ())->distance != 0){
                        pq.push(this->grid->graph.at(i).at(s->getY()).at(s->getZ()));
                    }
                }
            }
            else if(s->getLayer() == 1){
                for(int i = std::min(s->getY(), s->getNeighbor()); i <= std::max(s->getY(), s->getNeighbor()); i++){
                    if(this->grid->graph.at(s->getX()).at(i).at(s->getZ())->distance != 0){
                        pq.push(this->grid->graph.at(s->getX()).at(i).at(s->getZ()));
                    }
                }
            }
            else if(s->getLayer() == 2){
                for(int i = std::min(s->getZ(), s->getNeighbor()); i <= std::max(s->getZ(), s->getNeighbor()); i++){
                    if(this->grid->graph.at(s->getX()).at(s->getY()).at(i)->distance != 0){
                        pq.push(this->grid->graph.at(s->getX()).at(s->getY()).at(i));
                    }
                }
            }
            else{
                // Handle the case when getLayer() returns an unexpected value
                throw std::runtime_error("Invalid layer value");
            }
        }
    }
    // Set all vertex's prevertex to nullptr
    current_grid->setPrevertexNull();
    /* 
     * Dijkstra's algorithm for finding the shortest path in tree to tree.
     * The algorithm starts from the source tree and explores all reachable vertices,
     * until it reaches the sink or there are no more vertices left to explore.
     */
    while(!pq.empty()){
        current = pq.top(); pq.pop();
        if(current->is_sink){
            break;
        }
        // Enumerate 4 directions
        int direction = current->getLayer();
        int obstacle_id = layout->obstacle_id;
        for(unsigned i = 0; i < move_orientation.at(direction).size(); i++){
            auto next_x = current->coordinate.x + move_orientation.at(direction).at(i).x;
            auto next_y = current->coordinate.y + move_orientation.at(direction).at(i).y;
            auto next_z = current->coordinate.z + move_orientation.at(direction).at(i).z;
            if(outOfBound(Coordinate3D{next_x, next_y, next_z})) continue;
            if(current_grid->graph.at(next_x).at(next_y).at(next_z)->isObstacle(obstacle_id, net->id)) continue;
            if(current->distance + mazeRouteCost(this, current_grid, current->coordinate, Coordinate3D{next_x, next_y, next_z}) < current_grid->graph.at(next_x).at(next_y).at(next_z)->distance){
                current_grid->graph.at(next_x).at(next_y).at(next_z)->prevertex = current;
                current_grid->graph.at(next_x).at(next_y).at(next_z)->distance = current->distance + mazeRouteCost(this, current_grid, current->coordinate, Coordinate3D{next_x, next_y, next_z}); 
                pq.push(current_grid->graph.at(next_x).at(next_y).at(next_z));
            }
        }
    }
    /* Backtracking */
    // Failed tree2tree routing
    if(current == nullptr){
        throw std::runtime_error("No step was taken");
    }
    if(!current_grid->graph.at(current->coordinate.x).at(current->coordinate.y).at(current->coordinate.z)->is_sink){
        throw std::runtime_error("Not reach the sink");
    }

    

    // Tree2tree routing success
    Path tmp_path(net->id);
    tmp_path.start_pin = current->coordinate;
    Coordinate3D prev_coordinate = current->coordinate;
    current = current->prevertex;

    int prev_direction = -1;
    Segment* prev_segment = nullptr;

    while (current != nullptr) {
        Coordinate3D current_coordinate = current->coordinate;
        int direction = -1;

        if (prev_coordinate.x != current_coordinate.x) {
            direction = 0; // x-axis
        } else if (prev_coordinate.y != current_coordinate.y) {
            direction = 1; // y-axis
        } else if (prev_coordinate.z != current_coordinate.z) {
            direction = 2; // z-axis
        }

        if (direction != -1) {
            if (prev_direction == direction) {
                // If the direction is the same as the previous segment, update the end point of the previous segment
                prev_segment->end_point = current_coordinate;
            } else {
                // Create a new segment and push it to the segments vector
                prev_segment = new Segment(direction, prev_coordinate, current_coordinate);
                tmp_path.segments.push_back(prev_segment);
                prev_direction = direction;
            }
        }
        prev_coordinate = current_coordinate;
        current = current->prevertex;
    }

    tmp_path.end_pin = prev_coordinate;

    
    /* Post job cleanup */
    // Let the second point reset to not the sink
    for (const auto& pin : net->pins) {
        current_grid->resetSinks(pin);
    }
    for (const auto& path : net->tree->getPath()) {
        for(const auto& segment: path->segments) {
            current_grid->resetSinks(*segment);
        }
    }
    return tmp_path;
}