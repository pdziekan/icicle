# requireing C++11  TODO-check if the compiler supports it!

find_package(OpenMP)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

find_path(GNUPLOT-IOSTREAM_INCLUDE_DIR PATH_SUFFIXES gnuplot-iostream/ NAMES gnuplot-iostream.h)
#TODO: the same for blitz, libcloudph, libmpdata
#TODO: add the above paths to compiler flags

#
include(CheckCXXSourceCompiles)
check_cxx_source_compiles("#include <future>\n int main() { std::future<void> f; }\n" STD_FUTURE_WORKS)
if(${STD_FUTURE_WORKS})
  add_definitions(-DSTD_FUTURE_WORKS)
endif()

add_executable(icicle icicle.cpp)

# TODO: target_compile_options() // added to CMake on Jun 3rd 2013

find_package(Boost COMPONENTS thread iostreams system timer program_options filesystem REQUIRED)
target_link_libraries(icicle ${Boost_LIBRARIES})

#TODO: check if it's there
target_link_libraries(icicle cloudphxx_lgrngn)

find_package(HDF5 COMPONENTS CXX HL REQUIRED QUIET)
target_link_libraries(icicle ${HDF5_LIBRARIES})

install(TARGETS icicle DESTINATION bin)
