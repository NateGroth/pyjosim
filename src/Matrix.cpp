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
            py::keep_alive<1, 2>());
}

} // namespace pyjosim
