#include "./scoped_cout_null.hpp"

#include "JoSIM/Simulation.hpp"

#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>

#include <iostream>
#include <memory>

namespace py = pybind11;

namespace pyjosim {
void simulation(py::module &m)
{
    using namespace JoSIM;

    // Create the Simulation class
    py::class_<Simulation>(m, "Simulation")
        // Initialize the Simulation clas
        .def(py::init([](Input &input, Matrix &matrix) {
                 scoped_cout_null cout;
                 py::scoped_estream_redirect cerr;
                 // Construct on the heap and hand the holder the exact object
                 // that ran the transient. Returning by value let pybind copy/
                 // move it, which dropped the populated `results` (all-zero
                 // Output.traces). aether_sims fork fix.
                 return std::make_unique<Simulation>(input, matrix);
             }),
             py::keep_alive<1, 2>(), py::keep_alive<1, 3>());
}

} // namespace pyjosim
