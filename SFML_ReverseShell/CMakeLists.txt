# CMakeList.txt : CMake project for SFML_ReverseShell, include source and define
# project specific logic here.
#
# Declare SFML
project ("SFML_ReverseShell")
# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable static linking
set(SFML_STATIC_LIBRARIES TRUE)
set(BUILD_SHARED_LIBS FALSE)

# Optimize for release
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Static linking for GCC
if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
endif()

# Static runtime linking on MSVC
if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
endif()
include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML
    GIT_TAG 2.6.2
)

# Fetch SFML
FetchContent_MakeAvailable(SFML)

# Add source to this project's executable.
add_executable (SFML_ReverseShell "SFML_ReverseShell.cpp" "SFML_ReverseShell.h")
target_link_libraries(SFML_ReverseShell sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SFML_ReverseShell PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
