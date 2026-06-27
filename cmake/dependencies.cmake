include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)

CPMAddPackage(
  NAME pybind11
  GITHUB_REPOSITORY pybind/pybind11
  VERSION 2.10.4
)

# aether_sims monorepo wiring: build against the in-tree JoSIM fork submodule
# (aether_sims/extern/JoSIM) instead of fetching JoeyDelp/JoSIM. A relative path
# computed in-CMake avoids passing a space-containing path through setup.py's
# CMAKE_ARGS splitter. Picks up local fork edits (D0-D4) without a push.
get_filename_component(_AETHER_JOSIM_DIR "${CMAKE_CURRENT_LIST_DIR}/../../JoSIM" ABSOLUTE)
if(EXISTS "${_AETHER_JOSIM_DIR}/CMakeLists.txt")
  set(CPM_josim_SOURCE "${_AETHER_JOSIM_DIR}" CACHE PATH "Local JoSIM fork submodule")
endif()

CPMAddPackage(
  NAME josim
  GITHUB_REPOSITORY NateGroth/JoSIM
  GIT_TAG master
  OPTIONS
    "MAKING_STATIC_BUILD ON"
)