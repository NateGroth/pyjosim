#include "./scoped_cout_null.hpp"

#include "JoSIM/Simulation.hpp"

#include <pybind11/functional.h>
#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>

#include <functional>
#include <iostream>
#include <memory>

namespace py = pybind11;

namespace pyjosim {
void simulation(py::module &m)
{
    using namespace JoSIM;

    // Create the Simulation class
    py::class_<Simulation>(m, "Simulation")
        // Batch init: runs the whole transient (default). Constructed on the
        // heap so Python holds the exact object that ran (returning by value
        // dropped `results`; aether_sims fork fix).
        .def(py::init([](Input &input, Matrix &matrix) {
                 scoped_cout_null cout;
                 py::scoped_estream_redirect cerr;
                 return std::make_unique<Simulation>(input, matrix);
             }),
             py::keep_alive<1, 2>(), py::keep_alive<1, 3>())
        // Stepped / co-sim factory (aether_sims D4): setup + factor + startup,
        // but NOT the main loop. Drive with step(i, matrix) in a loop, then
        // finish() (or let the object be GC'd).
        .def_static(
            "stepped",
            [](Input &input, Matrix &matrix) {
                scoped_cout_null cout;
                py::scoped_estream_redirect cerr;
                return std::make_unique<Simulation>(input, matrix, true);
            },
            py::keep_alive<0, 1>(), py::keep_alive<0, 2>(), py::arg("input"),
            py::arg("matrix"))
        // One transient step; returns True if a timestep reduction is needed.
        .def(
            "step",
            [](Simulation &self, int64_t i, Matrix &matrix) {
                scoped_cout_null cout;
                py::scoped_estream_redirect cerr;
                return self.step(i, matrix);
            },
            py::arg("i"), py::arg("matrix"))
        // Run the whole main loop invoking the hooks (C++-callback co-sim).
        .def(
            "run_main",
            [](Simulation &self, Matrix &matrix) {
                scoped_cout_null cout;
                py::scoped_estream_redirect cerr;
                self.run_main(matrix);
            },
            py::arg("matrix"))
        .def("finish", &Simulation::finish)
        // D7 reduce_step support for the stepped path.
        .def("needs_reduction", &Simulation::needs_reduction)
        .def(
            "reduce_and_restart",
            [](Simulation &self, Input &input, Matrix &matrix) {
                scoped_cout_null cout;
                py::scoped_estream_redirect cerr;
                self.reduce_and_restart(input, matrix);
            },
            py::arg("input"), py::arg("matrix"))
        .def("sim_size", &Simulation::sim_size)
        .def("step_size", &Simulation::step_size)
        // Per-step co-simulation hooks (aether_sims D4): callables (i, t).
        .def("set_pre_step_hook",
             [](Simulation &self, std::function<void(int64_t, double)> f) {
                 self.pre_step_hook_ = std::move(f);
             })
        .def("set_post_step_hook",
             [](Simulation &self, std::function<void(int64_t, double)> f) {
                 self.post_step_hook_ = std::move(f);
             })
        .def("clear_hooks",
             [](Simulation &self) {
                 self.pre_step_hook_ = nullptr;
                 self.post_step_hook_ = nullptr;
             })
        // Per-junction electro-thermal access by label (aether_sims D1/D2).
        .def(
            "set_jj_temperature",
            [](Simulation &self, Matrix &matrix, const std::string &label,
               double T) { self.set_jj_temperature(matrix, label, T); },
            py::arg("matrix"), py::arg("label"), py::arg("T"))
        .def(
            "set_all_temperatures",
            [](Simulation &self, Matrix &matrix, double T) {
                self.set_all_temperatures(matrix, T);
            },
            py::arg("matrix"), py::arg("T"))
        .def(
            "jj_ic",
            [](Simulation &self, Matrix &matrix, const std::string &label) {
                return self.jj_ic(matrix, label);
            },
            py::arg("matrix"), py::arg("label"));
}

} // namespace pyjosim
