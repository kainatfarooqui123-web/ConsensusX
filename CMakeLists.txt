cmake_minimum_required(VERSION 3.14)
project(ConsensusX VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(consensusx
    src/main.cpp
    src/utils.cpp
    src/fasta.cpp
    src/consensus.cpp
    src/analysis.cpp
    src/similarity.cpp
    src/report.cpp
)

target_include_directories(consensusx PRIVATE src)
