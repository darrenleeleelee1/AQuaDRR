#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "grid.hpp"
#include "layout.hpp"
#include "router.hpp"
#include "io.hpp"

namespace py = pybind11;

// Custom conversion function for cur_paths member in the Vertex class
static py::list getCurPaths(const Vertex& vertex) {
    py::list paths;
    for (const auto& path : vertex.cur_paths) {
        paths.append(*path);
    }
    return paths;
}


PYBIND11_MODULE(router, m) {
    m.doc() = "pybind11 router plugin";

    // Bind the Path class
    py::class_<Path>(m, "Path")
        .def(py::init<>())
        .def(py::init<int>())
        .def(py::init<const Path&>())
        .def_readwrite("id", &Path::id)
        .def_readwrite("segments", &Path::segments)
        .def_readwrite("start_pin", &Path::start_pin)
        .def_readwrite("end_pin", &Path::end_pin);
        
    // Bind the Vertex class
    py::class_<Vertex>(m, "Vertex")
        .def(py::init<>())
        .def_readonly("coordinate", &Vertex::coordinate)
        // .def_readonly("cur_paths", &Vertex::cur_paths)
        .def_property_readonly("cur_paths", &getCurPaths)  // Use the custom conversion function
        .def_readwrite("prevertex", &Vertex::prevertex)
        .def_readwrite("distance", &Vertex::distance)
        .def_readwrite("obstacle", &Vertex::obstacle)
        .def_readwrite("is_sink", &Vertex::is_sink)
        .def("isObstacle", &Vertex::isObstacle)
        .def("getLayer", &Vertex::getLayer);

    // Bind the Grid class
    py::class_<Grid>(m, "Grid")
        .def(py::init<>())
        .def_readwrite("graph", &Grid::graph)
        .def_readwrite("history", &Grid::history);

    // Bind the Router class
    py::class_<Router>(m, "Router")
        .def(py::init<>())
        .def(py::init<Layout*>())
        .def_readwrite("grid", &Router::grid)
        .def("main", &Router::main)
        .def("initial_routing", &Router::initial_routing)
        .def("countNumOfViolation", &Router::countNumOfViolation)
        .def_readwrite("num_of_reroute", &Router::num_of_reroute)
        .def_readwrite("num_of_violation", &Router::num_of_violation);

    // Bind the Layout class
    py::class_<Layout>(m, "Layout")
        .def(py::init<>())
        .def(py::init<int, int, int, double, double, double>(), py::arg("width"), py::arg("height"), py::arg("num_of_layers"),
             py::arg("via_cost"), py::arg("horizontal_segment_cost"), py::arg("vertical_segment_cost"))
        .def_readwrite("width", &Layout::width)
        .def_readwrite("height", &Layout::height)
        .def_readwrite("num_of_layers", &Layout::num_of_layers)
        .def_readwrite("via_cost", &Layout::via_cost)
        .def_readwrite("horizontal_segment_cost", &Layout::horizontal_segment_cost)
        .def_readwrite("vertical_segment_cost", &Layout::vertical_segment_cost)
        .def("getWirelength", &Layout::getWirelength)
        .def("getCost", &Layout::getCost);

    m.def("readLayout", &io::readLayout, py::arg("layout"), py::arg("file_path"));
    m.def("writeLayout", &io::writeLayout, py::arg("layout"), py::arg("file_path"));
    m.def("tokenLine", &io::tokenLine, py::arg("tokens"), py::arg("line"));
}
