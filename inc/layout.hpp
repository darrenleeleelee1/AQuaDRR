#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <climits>
class Tree;
class Net;
class ViaCoordinate3D{
public:
	int x, y, z1, z2;
	ViaCoordinate3D(){}
	ViaCoordinate3D(int _x, int  _y, int  _z1, int _z2) : x(_x), y(_y), z1(_z1), z2(_z2) {
		if(z1 > z2) std::swap(z1, z2);
	}
	~ViaCoordinate3D(){}
	std::string toString(){
		std::string tmp = "";
		tmp += "(" + std::to_string(this->x) + "," + std::to_string(this->y) + "," + std::to_string(this->z1) + "," + std::to_string(this->z2) + ")";
		return tmp;
	}
	bool operator==(const ViaCoordinate3D &other) const {
    	return this->x == other.x && this->y == other.y && this->z1 == other.z1 && this->z2 == other.z2;
    }
	bool operator<(const ViaCoordinate3D &other) const {
		if (this->x != other.x)
			return this->x < other.x;
		if (this->y != other.y)
			return this->y < other.y;
		if (this->z1 != other.z1)
			return this->z1 < other.z1;
		return this->z2 < other.z2;
	}

};
class Coordinate3D{
public:
	int x, y, z;
	Coordinate3D(){}
	Coordinate3D(int _x, int  _y, int  _z) : x(_x), y(_y), z(_z) {}
	~Coordinate3D(){}
	int getLayer(){
		return z % 2;
	}
	std::string toString(){
		std::string tmp = "";
		tmp += "(" + std::to_string(this->x) + "," + std::to_string(this->y) + "," + std::to_string(this->z) + ")";
		return tmp;
	}
	bool operator==(const Coordinate3D &other) const {
    	return this->x == other.x && this->y == other.y && this->z == other.z;
    }
};
inline void hash_combine(std::size_t& seed, const int& v) {
	seed ^= std::hash<int>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
class Coordinate2D{
public:
	int x, y;
	Coordinate2D(){}
	Coordinate2D(Coordinate3D coor) : x(coor.x), y(coor.y) {} 
	Coordinate2D(int _x, int  _y) : x(_x), y(_y) {}
	~Coordinate2D(){}
	std::string toString(){
		std::string tmp = "";
		tmp += "(" + std::to_string(this->x) + "," + std::to_string(this->y) + ")";
		return tmp;
	}
	bool operator==(const Coordinate2D &other) const {
    	return this->x == other.x && this->y == other.y;
    }
	bool operator<(const Coordinate2D &other) const {
    	if (this->x == other.x)
        	return this->y < other.y;
        return this->x < other.x;
    }
};
namespace std {
    template<> struct hash<Coordinate3D> {
        size_t operator()(const Coordinate3D& c) const {
            size_t seed = 0;
            hash_combine(seed, c.x);
            hash_combine(seed, c.y);
            hash_combine(seed, c.z);
            return seed;
        }
    };

	template<> struct hash<Coordinate2D> {
        size_t operator()(const Coordinate2D& c) const {
            size_t seed = 0;
            hash_combine(seed, c.x);
            hash_combine(seed, c.y);
            return seed;
        }
    };
}
class Obstacle{
public:
	Coordinate3D start_point, end_point;
	Obstacle(){}
	Obstacle(int _spx, int  _spy, int  _spz, int _epx, int  _epy, int  _epz)
		: start_point(_spx, _spy, _spz), end_point(_epx, _epy, _epz) {}
	~Obstacle(){}
	bool operator==(const Obstacle &other) const {
    	return this->start_point == other.start_point && this->end_point == other.end_point;
    }
};
class Segment_Draw{
public:
	// If end_point == 0, means this segment is horizontal, sp.y == ep.y && sp.z == ep.z == 0
	// else if end_point == 1, means this segment is vertical, sp.x == ep.x && sp.z == ep.z == 1
	Coordinate3D start_point, end_point;
	Segment_Draw(){}
	Segment_Draw(int _spx, int  _spy, int  _spz, int _epx, int  _epy, int  _epz)
		: start_point(std::min(_spx, _epx), std::min(_spy, _epy), _spz)
			, end_point(std::max(_spx, _epx), std::max(_spy, _epy), _epz) {}
	~Segment_Draw(){}
	bool operator==(const Segment_Draw &other) const {
    	return this->start_point == other.start_point && this->end_point == other.end_point;
    }
};
class Segment{
public:
	int direction; // 0 for x, 1 for y, 2 for z
	Coordinate3D start_point;
	Coordinate3D end_point;
	Segment(){}
	Segment(int direction, Coordinate3D start_point, Coordinate3D end_point)
        : direction(direction), start_point(start_point), end_point(end_point) {}
	Segment(int _x, int _y, int _z, int _dir, int nei) {
		this->direction = _dir;
		if(getLayer() == 0){
			_x = std::min(_x, nei); nei = std::max(_x, nei);
			start_point = Coordinate3D{_x, _y, _z};
			end_point = Coordinate3D{nei, _y, _z};
		}
		else if(getLayer() == 1){
			_y = std::min(_y, nei); nei = std::max(_y, nei);
			start_point = Coordinate3D{_x, _y, _z};
			end_point = Coordinate3D{_x, nei, _z};
		}
		else if(getLayer() == 2){
			_z = std::min(_z, nei); nei = std::max(_z, nei);
			start_point = Coordinate3D{_x, _y, _z};
			end_point = Coordinate3D{_x, _y, nei};
		}
		else{
			// Handle the case when getLayer() returns an unexpected value
			throw std::runtime_error("Invalid layer value");
		}
	}
	int getLayer(){
		return direction;
	}
	int getX(){
		if(getLayer() == 0) return std::min(start_point.x, end_point.x);
		else return start_point.x; // y and z direction, sp's and ep's x should be the same
	}
	int getY(){
		if(getLayer() == 1) return std::min(start_point.y, end_point.y);
		else return start_point.y; // y and z direction, sp's and ep's y should be the same
	}
	int getZ(){
		if(getLayer() == 2) return std::min(start_point.z, end_point.z);
		else return start_point.z; // x and y direction, sp's and ep's z should be the same
	}
	int getNeighbor(){
		if(getLayer() == 0) return std::max(start_point.x, end_point.x);
		else if(getLayer() == 1) return std::max(start_point.y, end_point.y);
		else return std::max(start_point.z, end_point.z);
	}
	void setX(int x){
		if(getLayer() == 0) {
			start_point.x = std::min(x, end_point.x);
			end_point.x = std::max(x, end_point.x);
		}
		else {
			start_point.x = x;
			end_point.x = x;
		}
	}
	void setY(int y){
		if(getLayer() == 1) {
			start_point.y = std::min(y, end_point.y);
			end_point.y = std::max(y, end_point.y);
		}
		else {
			start_point.y = y;
			end_point.y = y;
		}
	}
	void setZ(int z){
		if(getLayer() == 2) {
			start_point.z = std::min(z, end_point.z);
			end_point.z = std::max(z, end_point.z);
		}
		else {
			start_point.z = z;
			end_point.z = z;
		}
	}
	void setNeighbor(int nei){
		if(getLayer() == 0){
			start_point.x = std::min(nei, end_point.x);
			end_point.x = std::max(nei, end_point.x);
		}
		else if(getLayer() == 1){
			start_point.y = std::min(nei, end_point.y);
			end_point.y = std::max(nei, end_point.y);
		}
		else if(getLayer() == 2){
			start_point.z = std::min(nei, end_point.z);
			end_point.z = std::max(nei, end_point.z);
		}
		else{
			// Handle the case when getLayer() returns an unexpected value
			throw std::runtime_error("Invalid layer value");
		}
	}
	Coordinate3D startPoint(){
		return Coordinate3D{getX(), getY(), getZ()};
	}
	Coordinate3D endPoint(){
		if(getLayer() == 0) return Coordinate3D{getNeighbor(), getY(), getZ()};
		else if(getLayer() == 1) return Coordinate3D{getX(), getNeighbor(), getZ()};
		else if(getLayer() == 2) return Coordinate3D{getX(), getY(), getNeighbor()};
		else{
			// Handle the case when getLayer() returns an unexpected value
        	throw std::runtime_error("Invalid layer value");
		}
	}
	std::string toString(){
		auto sp = startPoint(), ep = endPoint();
		return "(" + std::to_string(sp.x) + "," + std::to_string(sp.y) + "," + std::to_string(sp.z) + ")"
				+ "-(" + std::to_string(ep.x) + "," + std::to_string(ep.y) + "," + std::to_string(ep.z) + ")";
	}
	bool colinear(Coordinate3D point){
		if(getLayer() == 0){
			if(getY() != point.y || getZ() != point.z) return false;
			if(getX() <= point.x && point.x <= getNeighbor()) return true;
		}
		else if(getLayer() == 1){
			if(getX() != point.x || getZ() != point.z) return false;
			if(getY() <= point.y && point.y <= getNeighbor()) return true;
		}
		else if(getLayer() == 2){
			if(getX() != point.x || getY() != point.y) return false;
			if(getZ() <= point.z && point.z <= getNeighbor()) return true;
		}
		else{
			// Handle the case when getLayer() returns an unexpected value
        	throw std::runtime_error("Invalid layer value");
		}
		return false;
	}
	int getWirelength(){
		if(getLayer() == 0) return getNeighbor() - getX();
		else if(getLayer() == 1) return getNeighbor() - getY();
		else if(getLayer() == 2) return getNeighbor() - getZ();
		else{
			// Handle the case when getLayer() returns an unexpected value
        	throw std::runtime_error("Invalid layer value");
		}
	}
	double getCost(double horizontal_cost, double vertical_cost, double via_cost){
		if(getLayer() == 0) return getWirelength() * horizontal_cost;
		else if(getLayer() == 1) return getWirelength() * vertical_cost;
		else if(getLayer() == 2) return getWirelength() * via_cost;
		else{
			// Handle the case when getLayer() returns an unexpected value
        	throw std::runtime_error("Invalid layer value");
		}
	}
};
class Path{
public:
	int id;
	std::vector<Segment*> segments;
	Coordinate3D start_pin, end_pin;
	Path() {
		this->segments.resize(0);
	}
	Path(int _id){
		this->id = _id;
		this->segments.resize(0);
	}
	Path(const Path& other) {
		this->id = other.id;
        this->segments.resize(0);
        for (auto s : other.segments) {
            this->segments.push_back(new Segment(*s));
        }
        this->start_pin = other.start_pin;
        this->end_pin = other.end_pin;
    }
	~Path() {
		for (auto s : segments) {
			delete s;
		}
	}
	void lineUpSegments(){
		Coordinate2D sp = Coordinate2D{start_pin};
		unsigned swap_index;
		for(unsigned i = 0; i < segments.size(); i++){
			swap_index = i;
			for(unsigned j = i; j < segments.size(); j++){
				if(Coordinate2D{sp} == Coordinate2D{segments.at(j)->startPoint()} || Coordinate2D{sp} == Coordinate2D{segments.at(j)->endPoint()}){
					swap_index = j;
					break;
				}
			}
			std::swap(segments.at(i), segments.at(swap_index));
			auto &s = segments.at(i);
			sp = ((Coordinate2D{s->startPoint()} == Coordinate2D{sp}) ? Coordinate2D{s->endPoint()} : Coordinate2D{s->startPoint()});
		}
	}
};
class Subtree{
public:
    std::vector<Path*> paths; // stores the paths in each subtree
	std::unordered_set<Coordinate3D> pinlist;
	
	Subtree() {}
	Subtree(Coordinate3D pin) {
		paths.resize(0);
		this->pinlist.insert(pin);
	}
	~Subtree(){
		for(auto e : paths){
			delete e;
		}
	}
	std::string showPins(){
		std::string tmp;
		for(auto p : pinlist) tmp += p.toString() + ", ";
		return tmp;
	}
};
class Tree{
public:
    std::vector<int> parents; // stores the parent of each node in the subtree
    std::vector<Subtree*> subtrees;
    std::unordered_map<Coordinate3D, int> coordinate2index;
    std::unordered_set<Coordinate3D> pinset;
    Tree() {}
    Tree(std::vector<Coordinate3D> pins) {
        pinset.insert(pins.begin(), pins.end());
        unsigned n = pins.size();
        this->parents.resize(n, -1); // Initialize all roots with -1 nodes
        subtrees.resize(n);
        for (unsigned i = 0; i < n; i++){
            subtrees.at(i) = (new Subtree(pins.at(i)));
            this->coordinate2index[Coordinate3D{pins.at(i)}] = i;
        }
    }
	~Tree(){
		for(unsigned i = 0; i < subtrees.size(); i++){
			delete subtrees.at(i);
		}
	}
	
	Subtree* at(int index){
		return subtrees.at(find(index));
	}

	int find(int x) {
		while (parents.at(x) >= 0) x = parents.at(x);
		return x;
	}

	bool mergeTree(int x, int y) {
		int x_root = find(x);
		int y_root = find(y);
		if (x_root != y_root) {
			if (parents.at(x_root) > parents.at(y_root)) {
				std::swap(x_root, y_root);
			}
			parents.at(x_root) += parents.at(y_root); // Merge the sizes of the two trees
			parents.at(y_root) = x_root;
			// merge paths
			subtrees.at(x_root)->paths.insert(subtrees.at(x_root)->paths.end(), subtrees.at(y_root)->paths.begin(), subtrees.at(y_root)->paths.end());
			subtrees.at(y_root)->paths.clear();
			// merge pinlist
			subtrees.at(x_root)->pinlist.insert(subtrees.at(y_root)->pinlist.begin(), subtrees.at(y_root)->pinlist.end());
			subtrees.at(y_root)->pinlist.clear();
			return true;
		}
		return false;
	}

	int isConnect() {
		int min_size = INT_MAX;
		int min_root = -1;
		int negative_count = 0;

		for (size_t i = 0; i < parents.size(); ++i) {
			if (parents.at(i) < 0) {
				negative_count++;
				if (-parents.at(i) < min_size) {
					min_size = -parents.at(i);
					min_root = i;
				}
			}
		}

		// Return -1 if there's only one negative value left in parents
		if (negative_count == 1) {
			return -1;
		}

		return min_root;
	}


	int findSubtree(const Coordinate3D& coord){
		std::unordered_set<int> roots;
		for(unsigned i = 0; i < coordinate2index.size(); i++){
			int root = find(i);
			if(!roots.count(root)){
				roots.insert(root);
				if(subtrees.at(root)->pinlist.count(coord)) return root;
				for (const auto& path : subtrees.at(root)->paths) {
					for (const auto& segment : path->segments) {
						Coordinate3D start = segment->startPoint();
						Coordinate3D end = segment->endPoint();
						bool x_in_range = (start.x <= coord.x && coord.x <= end.x) || (start.x >= coord.x && coord.x >= end.x);
						bool y_in_range = (start.y <= coord.y && coord.y <= end.y) || (start.y >= coord.y && coord.y >= end.y);
						bool z_in_range = (start.z <= coord.z && coord.z <= end.z) || (start.z >= coord.z && coord.z >= end.z);

						if (x_in_range && y_in_range && z_in_range) {
							return root;
						}
					}
				}
			}
		}
		// Throw a runtime error if no candidate is found
    	throw std::runtime_error("No candidate found for the given coordinate.");
		return -1;
	}

	std::vector<Path*> getPath(){
		std::unordered_set<int> roots;
		std::vector<Path*> all_paths;
		for(unsigned i = 0; i < coordinate2index.size(); i++){
			int root = find(i);

			if(!roots.count(root)){
				roots.insert(root);
				all_paths.insert(all_paths.end(), subtrees.at(root)->paths.begin(), subtrees.at(root)->paths.end());
				// break; // Assume correct, it will only have one root
			}
		}
		return all_paths;
	}
};
class Net{
public:
	// For Input
	int id;
	std::vector<Coordinate3D> pins;
	// For Ouput
	std::set<ViaCoordinate3D> vialist;
	// Segment for drawing
	std::vector<Segment_Draw> horizontal_segments;
	std::vector<Segment_Draw> vertical_segments;
	Tree* tree = nullptr;
	Net(){
		id = -1;
		this->pins.resize(0);
		this->vialist.clear();
		this->horizontal_segments.resize(0);
		this->vertical_segments.resize(0);
	}
	Net(int _id) : id(_id){
		this->pins.resize(0);
		this->vialist.clear();
		this->horizontal_segments.resize(0);
		this->vertical_segments.resize(0);
	}
	~Net(){
		delete tree;
	}
	void initTrees(){
		tree = new Tree(pins);
	}
	double getCost(double horizontal_cost, double vertical_cost, double via_cost){
		double sum = 0;
		for(auto &p : tree->getPath()){
			for(auto &s : p->segments){
				sum += s->getCost(horizontal_cost, vertical_cost, via_cost);
			}
		}
		return sum;
	}
	int getWirelength(){
		int sum = 0;
		for(auto &p : tree->getPath()){
			for(auto &s : p->segments){
				sum += s->getWirelength();
			}
		}
		return sum;
	}
	bool checkIsPin(Coordinate3D target){
		for(auto p : pins){
			if(p == target) return true;
		}
		return false;
	}
	int isConnect(){
		return tree->isConnect();
	}
	// including adding via
	void segmentRegularize(){
		for(auto &p : tree->getPath()){
			for(auto &s : p->segments){
				if(s->getLayer() == 0){
					this->horizontal_segments.emplace_back(s->getX(), s->getY(), s->getZ(), s->getNeighbor(), s->getY(), s->getZ());
				}
				else if(s->getLayer() == 1){
					this->vertical_segments.emplace_back(s->getX(), s->getY(), s->getZ(), s->getX(), s->getNeighbor(), s->getZ());
				}
				else if(s->getLayer() == 2){
					this->vialist.emplace(s->getX(), s->getY(), s->start_point.z, s->end_point.z);
				}
				else{
					throw std::runtime_error("Invalid layer value");
				}
			}
		}
	}
};
class Layout{
public:
	// For Input
	int width, height;
	int num_of_layers = 2;
	double via_cost = 1;
	double horizontal_segment_cost = 1, vertical_segment_cost = 1;
    int obstacle_id; // Mex of net ids
	std::vector<Obstacle> obstacles;
	std::vector<Net> netlist;
	
	Layout(){}
	Layout(int w, int h, int nol, double vc, double hsc, double vsc)
		: width(w), height(h), num_of_layers(nol), via_cost(vc)
			, horizontal_segment_cost(hsc), vertical_segment_cost(vsc) {}
	~Layout(){}

	int getWirelength(){
		int sum = 0;
		for(unsigned i = 0; i < this->netlist.size(); i++){
			sum += this->netlist.at(i).getWirelength();
		}
		return sum;
	}

	double getCost(){
		double sum = 0;
		for(unsigned i = 0; i < this->netlist.size(); i++){
			sum += this->netlist.at(i).getCost(horizontal_segment_cost, vertical_segment_cost, via_cost);
		}
		return sum;
	}

	// debug
	void writeForDebug(unsigned test_num){
		char file_path[64]; 
		sprintf(file_path, "out/tmp_%u.txt", test_num);
		std::ofstream out_file(file_path, std::ofstream::trunc);
		out_file << "Width " << this->width << "\n";
		out_file << "Height " << this->height << "\n";
		out_file << "Layer " << this->num_of_layers << "\n";
		out_file << "Total_WL " << 0 << "\n";
		out_file << "Cost " << 0 << "\n";
		out_file << "Obstacle_num " << this->obstacles.size() << "\n";
		for(unsigned i = 0; i < this->obstacles.size(); i++){
			out_file << this->obstacles.at(i).start_point.x << " " << this->obstacles.at(i).start_point.y << " " << this->obstacles.at(i).start_point.z << " ";
			if(this->obstacles.at(i).start_point.z == 0)
				out_file << this->obstacles.at(i).end_point.x << " " << this->obstacles.at(i).end_point.y + 1<< " " << this->obstacles.at(i).end_point.z;
			else
				out_file << this->obstacles.at(i).end_point.x + 1 << " " << this->obstacles.at(i).end_point.y<< " " << this->obstacles.at(i).end_point.z;
			out_file << "\n";
		}
		out_file << "Net_num " << 1 << "\n";
		for(unsigned i = test_num; i <= test_num; i++){
			// debug
			this->netlist.at(i).horizontal_segments.clear();
			this->netlist.at(i).vertical_segments.clear();
			this->netlist.at(i).vialist.clear();
			// debug
			this->netlist.at(i).segmentRegularize();
			out_file << "Net_id " << this->netlist.at(i).id << "\n";
			out_file << "pin_num " << this->netlist.at(i).pins.size() << "\n";
			for(unsigned j = 0; j < this->netlist.at(i).pins.size(); j++){
				out_file << this->netlist.at(i).pins.at(j).x << " " << this->netlist.at(i).pins.at(j).y << " " << this->netlist.at(i).pins.at(j).z << "\n";
			}
			out_file << "Via_num " << this->netlist.at(i).vialist.size() << "\n";
			for(auto &v : this->netlist.at(i).vialist){
				out_file << v.x << " " << v.y << " " << v.z1 << " " << v.z2 << "\n";
			}
			out_file << "H_segment_num " << this->netlist.at(i).horizontal_segments.size() << "\n";
			for(unsigned j = 0; j < this->netlist.at(i).horizontal_segments.size(); j++){
				out_file << this->netlist.at(i).horizontal_segments.at(j).start_point.x << " " << this->netlist.at(i).horizontal_segments.at(j).start_point.y 
						<< " " << this->netlist.at(i).horizontal_segments.at(j).start_point.z << " ";
				out_file << this->netlist.at(i).horizontal_segments.at(j).end_point.x + 1 << " " << this->netlist.at(i).horizontal_segments.at(j).end_point.y + 1
						<< " " << this->netlist.at(i).horizontal_segments.at(j).end_point.z << "\n";
			}
			out_file << "V_segment_num " << this->netlist.at(i).vertical_segments.size() << "\n";
			for(unsigned j = 0; j < this->netlist.at(i).vertical_segments.size(); j++){
				out_file << this->netlist.at(i).vertical_segments.at(j).start_point.x << " " << this->netlist.at(i).vertical_segments.at(j).start_point.y 
						<< " " << this->netlist.at(i).vertical_segments.at(j).start_point.z << " ";
				out_file << this->netlist.at(i).vertical_segments.at(j).end_point.x + 1 << " " << this->netlist.at(i).vertical_segments.at(j).end_point.y + 1
						<< " " << this->netlist.at(i).vertical_segments.at(j).end_point.z << "\n";
			}
		}
	}
	// debug
};