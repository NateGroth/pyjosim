#include "JoSIM/Output.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <memory>

namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(std::vector<JoSIM::Trace>);

namespace pyjosim {
void output(py::module &m)
{
    using namespace JoSIM;
    
    // Create Trace class
    py::class_<Trace>(m, "Trace")
        // Expose type member
        .def_readonly("type", &Trace::type_)
        // Expose name member
        .def_property_readonly("name", [](const Trace &trace) {
            auto &&name = trace.name_;
            if (name.front() == '"' &&
                name.back() == '"' && name.size() >= 2) {
                return std::string(name.begin() + 1, name.begin() + name.size() - 1);
            }
            return std::string(name);
        })
        // Expose data member. Upstream returned `py::array_t<double>(size,
        // ptr)` with return_value_policy::reference_internal, which yielded an
        // all-zero array even though trace.data_ held the correct values.
        // aether_sims fork fix: return the vector and let pybind's STL caster
        // copy it element-wise (np.asarray() on the caller side gives a numpy
        // array). Correctness over the raw-buffer trick.
        .def_property_readonly("data", [](const Trace &trace) {
            return trace.data_;
        });

    // Expose traces vector
    py::bind_vector<std::vector<Trace>>(m, "Traces");

    // Create Output class
    py::class_<Output>(m, "Output")
        // Initialize output
        .def(py::init([](Input &input, Matrix &matrix, Simulation &simulation) {
                // Construct on the heap so Python holds the exact Output that
                // write_output filled. Returning by value copied/moved it and
                // dropped the trace data (all-zero traces). aether_sims fork fix.
                auto output = std::make_unique<Output>();
                output->write_output(input, matrix, simulation);
                return output;
             }),
             py::keep_alive<1, 2>(), py::keep_alive<1, 3>(),
             py::keep_alive<1, 4>())
        // Expose the output traces member
        .def_readonly("traces", &Output::traces, py::return_value_policy::reference_internal);
}

} // namespace pyjosim
