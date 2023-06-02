#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "router.hpp"
#include "io.hpp"
namespace py = pybind11;

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "router.hpp"
#include "layout.hpp"
#include "io.hpp"

namespace py = pybind11;

PYBIND11_MODULE(router, m) {
    m.doc() = "pybind11 router plugin";

    py::class_<Router>(m, "Router")
        .def(py::init<>())
        .def(py::init<Layout*>())
        .def("main", &Router::main)
        .def("countNumOfViolation", &Router::countNumOfViolation)
        .def_readwrite("num_of_reroute", &Router::num_of_reroute)
        .def_readwrite("num_of_violation", &Router::num_of_violation);


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


/*
PYBIND11_MODULE(router, m) {
    py::class_<Router>(m, "Router")
        .def(py::init<>())
        .def(py::init<Layout *>())
        .def_readwrite("layout", &Router::layout)
        .def_readwrite("grid", &Router::grid)
        .def_readwrite("pin_and_obstacle_grid", &Router::pin_and_obstacle_grid)
        .def_readwrite("history_cost", &Router::history_cost)
        .def_readwrite("overlapped_cost", &Router::overlapped_cost)
        .def_readwrite("num_of_reroute", &Router::num_of_reroute)
        .def_readwrite("num_of_violation", &Router::num_of_violation)
        .def("main", &Router::main)
        .def("initial_routing", &Router::initial_routing)
        .def("outOfBound", &Router::outOfBound)
        .def("tree2treeMazeRouting", &Router::tree2treeMazeRouting)
        .def("addHistoryCost", &Router::addHistoryCost)
        .def("insertPathsToGrid", &Router::insertPathsToGrid)
        .def("countNumOfViolation", &Router::countNumOfViolation);

    // Bind other classes such as Layout, Grid, Path etc. 
    // if they aren't already bound.
}
*/
