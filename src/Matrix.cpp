#include "./scoped_cout_null.hpp"

#include "JoSIM/Matrix.hpp"

#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>

#include <memory>
namespace py = pybind11;

namespace pyjosim {
void matrix(py::module &m)
{
    using namespace JoSIM;

    // Create Matrix class
    py::class_<Matrix>(m, "Matrix")
        // Initialize matrix class
        .def(py::init([](Input &input) {
                scoped_cout_null cout;
                py::scoped_estream_redirect cerr;
                // Construct on the heap so the matrix state (relevant traces,
                // branch index) is the exact object Python holds, not a copy.
                auto matrix = std::make_unique<Matrix>();
                matrix->create_matrix(input);
                find_relevant_traces(input, *matrix);
                return matrix;
            }),
            py::keep_alive<1, 2>())
        // aether_sims T35: how the run-level .ctrltau field was applied --
        // coupled junctions that took the field / kept a card CTRLLAG / run
        // memoryless (the record's D8 law).
        .def_readonly("ctrl_tau_field", &Matrix::ctrlTauField)
        .def_readonly("ctrl_tau_card", &Matrix::ctrlTauCard)
        .def_readonly("ctrl_tau_none", &Matrix::ctrlTauNone);
}

} // namespace pyjosim
