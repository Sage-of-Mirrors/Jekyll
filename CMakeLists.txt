cmake_minimum_required(VERSION 3.12)
project(jekyll)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GLFW3 REQUIRED)

add_subdirectory(lib)

file(GLOB JEKYLL_SRC
    # Jekyll
    "src/*.cpp"
    "include/*.hpp"
    "src/application/*.cpp"
    "include/application/*.hpp"
    "src/model/*.cpp"
    "include/model/*.hpp"
    "src/ui/*.cpp"
    "include/ui/*.hpp"
    
    # bStream
    "lib/bStream/*.cpp"
    
    # glad
    "lib/glad/src/*.c"

    # ImGuiFileDialog
    "lib/ImGuiFileDialog/ImGuiFileDialog.cpp"
)

add_executable(jekyll ${JEKYLL_SRC})

target_include_directories(jekyll PUBLIC include lib/glad/include lib/bStream lib/glm lib/ImGuiFileDialog)
target_link_libraries(jekyll PUBLIC glm j3dultra imgui glfw)
