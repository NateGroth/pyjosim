#include <pybind11/pybind11.h>

#include <cstdlib>

namespace py = pybind11;

// Define submodules
namespace pyjosim {

void analysis_type(py::module &m);
void cli_options(py::module &m);
void input(py::module &m);
void matrix(py::module &m);
void netlist(py::module &m);
void output(py::module &m);
void parameter_name(py::module &m);
void parameters(py::module &m);
void simulation(py::module &m);
void stl(py::module &m);

}; // namespace pyjosim

// Define python module
PYBIND11_MODULE(pyjosim, m)
{
    using namespace pyjosim;

    m.doc() = "JoSIM python interface";

    // aether_sims 4TJJ campaign: deterministic Langevin noise. The engine's
    // NOISE() sources draw from the process-global rand() stream, which
    // Input::parse_input seeds from wall-clock time (srand(time(NULL))).
    // Re-seeding AFTER parse and BEFORE Matrix/Simulation construction makes
    // a noisy transient bit-reproducible for a given seed.
    m.def("seed_noise", [](unsigned int seed) { std::srand(seed); },
          "Seed the engine's global noise RNG (call after Input.parse_input, "
          "before constructing Simulation).");

    analysis_type(m);
    cli_options(m);
    input(m);
    matrix(m);
    netlist(m);
    output(m);
    parameter_name(m);
    parameters(m);
    simulation(m);
    stl(m);
}
