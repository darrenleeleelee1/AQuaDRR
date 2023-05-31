#pragma once
#include <float.h> // for DBL_MAX
#include <vector>
#include <cstdint>
#include "layout.hpp"
class Vertex{
public:
    Coordinate3D coordinate;
    std::vector<Path*> cur_paths;
    Vertex *prevertex = nullptr;
    double distance = DBL_MAX;
    int obstacle = -1;
    bool is_sink = false;
    Vertex(){}
    Vertex(int x, int y, int z, bool o, bool s) 
        : coordinate(x, y, z), obstacle(-1), is_sink(s) {}
    ~Vertex() {}
    bool isObstacle(int obstacle_id, int pin_id){
        if(obstacle == -1) return false;
        else if(obstacle == obstacle_id) return true;
        else if(obstacle == pin_id) return false;
        return true;
    }
    
    int getLayer(){
		return coordinate.z % 2;
	}
};
class Grid{
public:
    std::vector<std::vector<std::vector<Vertex*>>> graph;
    std::vector<std::vector<std::vector<double>>> history;
    Grid() {}
    Grid(int width, int height, int layer){
        
        graph.resize(static_cast<unsigned>(width));
        history.resize(static_cast<unsigned>(width));

        for(int i = 0; i < width; i++){
            graph.at(i).resize(height);
            history.at(i).resize(height);
        }
        
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                graph.at(i).at(j).resize(layer);
                history.at(i).at(j).resize(layer);
                for(int k = 0; k < layer; k++){
                    graph.at(i).at(j).at(k) = new Vertex{i, j, k, false, false};
                    history.at(i).at(j).at(k) = 0.0;
                }
            }
        }
    }
    Grid(Layout *l){
        int width = l->width, height = l->height;
        int layer = l->num_of_layers;
        
        graph.resize(static_cast<unsigned>(width));
        history.resize(static_cast<unsigned>(width));

        for(int i = 0; i < width; i++){
            graph.at(i).resize(height);
            history.at(i).resize(height);
        }
        
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                graph.at(i).at(j).resize(layer);
                history.at(i).at(j).resize(layer);
                for(int k = 0; k < layer; k++){
                    graph.at(i).at(j).at(k) = new Vertex{i, j, k, false, false};
                    history.at(i).at(j).at(k) = 0.0;
                }
            }
        }
        // Set the obstacles and pins
        for(auto o : l->obstacles) this->setObstacles(l->netlist.size(), o.start_point, o.end_point);
        for(auto _n : l->netlist){
            for(auto p : _n.pins) this->setObstacles(_n.id, p, p);
        }
    }
    ~Grid() {
        for (unsigned i = 0; i < graph.size(); i++) {
            for (unsigned j = 0; j < graph.at(i).size(); j++) {
                for (unsigned k = 0; k < graph.at(i).at(j).size(); k++) {
                    delete graph.at(i).at(j).at(k);
                }
            }
        }

    }
    void setPrevertexNull(){
        for (unsigned i = 0; i < graph.size(); i++) {
            for (unsigned j = 0; j < graph[i].size(); j++) {
                for (unsigned k = 0; k < graph[i][j].size(); k++) {
                    this->graph.at(i).at(j).at(k)->prevertex = nullptr;
                }
            }
        }
    }
    void setDistanceInfinity(){
        for (unsigned i = 0; i < graph.size(); i++) {
            for (unsigned j = 0; j < graph[i].size(); j++) {
                for (unsigned k = 0; k < graph[i][j].size(); k++) {
                    this->graph.at(i).at(j).at(k)->distance = INT32_MAX;
                }
            }
        }
    }
    void setDistanceZero(Coordinate3D coor){
        this->graph.at(coor.x).at(coor.y).at(coor.z)->distance = 0;
    }
    void setDistanceZero(std::vector<Coordinate3D> coors){
        for(auto c : coors){
            setDistanceZero(c);
        }
    }
    void setDistanceZero(Segment seg){
        if(seg.getLayer() == 0){
            for(int i = seg.getX(); i <= seg.getNeighbor(); i++){
                this->graph.at(i).at(seg.getY()).at(seg.getZ())->distance = 0;
            }
        }
        else if(seg.getLayer() == 1){
            for(int i = seg.getY(); i <= seg.getNeighbor(); i++){
                this->graph.at(seg.getX()).at(i).at(seg.getZ())->distance = 0;
            }
        }
        else if(seg.getLayer() == 2){
            for(int i = seg.getZ(); i <= seg.getNeighbor(); i++){
                this->graph.at(seg.getX()).at(seg.getY()).at(i)->distance = 0;
            }
        }
    }
    void setDistanceZero(std::vector<Segment> segs){
        for(auto s : segs){
            setDistanceZero(s);
        }
    }
    void setObstacles(int net_id, Segment seg){
        if(seg.getLayer() == 0){
            for(int i = seg.getX(); i <= seg.getNeighbor(); i++){
                this->graph.at(i).at(seg.getY()).at(seg.getZ())->obstacle = net_id;
            }
        }
        else if(seg.getLayer() == 1){
            for(int i = seg.getY(); i <= seg.getNeighbor(); i++){
                this->graph.at(seg.getX()).at(i).at(seg.getZ())->obstacle = net_id;
            }
        }
        else if(seg.getLayer() == 2){
            for(int i = seg.getZ(); i <= seg.getNeighbor(); i++){
                this->graph.at(seg.getX()).at(seg.getY()).at(i)->obstacle = net_id;
            }
        }
    }
    void setObstacles(int net_id, Coordinate3D start_point, Coordinate3D end_point){
        // pins
        if(start_point == end_point){
            this->graph.at(start_point.x).at(start_point.y).at(start_point.z)->obstacle = net_id;
        }
        else{
            // Horizontal
            if(start_point.y == end_point.y){
                for(int i = std::min(start_point.x, end_point.x); i <= std::max(start_point.x, end_point.x); i++){
                    this->graph.at(i).at(start_point.y).at(start_point.z)->obstacle = net_id;
                }
            }
            // Verticle
            else if(start_point.x == end_point.x){
                for(int i = std::min(start_point.y, end_point.y); i <= std::max(start_point.y, end_point.y); i++){
                    this->graph.at(start_point.x).at(i).at(start_point.z)->obstacle = net_id;
                }
            }
        }
    }
    void setObstacles(int net_id, Coordinate3D pin){
        this->graph.at(pin.x).at(pin.y).at(pin.z)->obstacle = net_id;
    }
    void setObstacles(int net_id, std::vector<Coordinate3D> pins){
        for(auto v : pins){
            setObstacles(net_id, v);
        }
    }
    void resetObstacles(Segment seg){
        if(seg.getLayer() == 0){
            for(int i = seg.getX(); i <= seg.getNeighbor(); i++){
                this->graph.at(i).at(seg.getY()).at(seg.getZ())->obstacle = -1;
            }
        }
        else if(seg.getLayer() == 1){
            for(int i = seg.getY(); i <= seg.getNeighbor(); i++){
                this->graph.at(seg.getX()).at(i).at(seg.getZ())->obstacle = -1;
            }
        }
        else if(seg.getLayer() == 2){
            for(int i = seg.getZ(); i <= seg.getNeighbor(); i++){
                this->graph.at(seg.getX()).at(seg.getY()).at(i)->obstacle = -1;
            }
        }
    }
    void resetObstacles(Coordinate3D pin){
        this->graph.at(pin.x).at(pin.y).at(pin.z)->obstacle = -1;
    }
    void resetObstacles(std::vector<Coordinate3D> pins){
        for(auto v : pins){
            resetObstacles(v);
        }
    }
    void setSinks(Segment seg){
        if(seg.getLayer() == 0){
            for(int i = seg.getX(); i <= seg.getNeighbor(); i++){
                this->graph.at(i).at(seg.getY()).at(seg.getZ())->is_sink = true;
            }
        }
        else if(seg.getLayer() == 1){
            for(int i = seg.getY(); i <= seg.getNeighbor(); i++){
                this->graph.at(seg.getX()).at(i).at(seg.getZ())->is_sink = true;
            }
        }
        else if(seg.getLayer() == 2){
            for(int i = seg.getZ(); i <= seg.getNeighbor(); i++){
                this->graph.at(seg.getX()).at(seg.getY()).at(i)->is_sink = true;
            }
        }
    }
    void setSinks(std::vector<Segment> segs){
        for(auto s : segs){
            setSinks(s);
        }
    }
    void setSinks(Coordinate3D coor){
        this->graph.at(coor.x).at(coor.y).at(coor.z)->is_sink = true;
    }
    void setSinks(std::vector<Coordinate3D> pins){
        for(auto v : pins){
            setSinks(v);
        }
    }
    void resetSinks(Segment seg){
        if(seg.getLayer() == 0){
            for(int i = seg.getX(); i <= seg.getNeighbor(); i++){
                this->graph.at(i).at(seg.getY()).at(seg.getZ())->is_sink = false;
            }
        }
        else if(seg.getLayer() == 1){
            for(int i = seg.getY(); i <= seg.getNeighbor(); i++){
                this->graph.at(seg.getX()).at(i).at(seg.getZ())->is_sink = false;
            }
        }
        else if(seg.getLayer() == 2){
            for(int i = seg.getZ(); i <= seg.getNeighbor(); i++){
                this->graph.at(seg.getX()).at(seg.getY()).at(i)->is_sink = false;
            }
        }
    }
    void resetSinks(std::vector<Segment> segs){
        for(auto s : segs){
            resetSinks(s);
        }
    }
    void resetSinks(Coordinate3D coor){
        this->graph.at(coor.x).at(coor.y).at(coor.z)->is_sink = false;
    }
    void resetSinks(std::vector<Coordinate3D> pins){
        for(auto v : pins){
            resetSinks(v);
        }
    }
};
