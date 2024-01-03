cmake_minimum_required(VERSION 3.12)
project(jekyll)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GLFW3 REQUIRED)
find_package(magic_enum REQUIRED)

add_subdirectory(lib)

file(GLOB JEKYLL_SRC
    # Jekyll
    "src/*.cpp"
    "include/*.hpp"
    "src/application/*.cpp"
    "include/application/*.hpp"
    "include/img/*.h"
    "src/model/*.cpp"
    "include/model/*.hpp"
    "src/ui/*.cpp"
    "include/ui/*.hpp"
    "src/ui/properties/*.cpp"
    "include/ui/properties/*.hpp"
    
    # bStream
    "lib/bStream/*.cpp"
    
    # glad
    "lib/glad/src/*.c"

    # ImGuiFileDialog
    "lib/ImGuiFileDialog/ImGuiFileDialog.cpp"
)

add_executable(jekyll ${JEKYLL_SRC})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/asset/font/MaterialSymbolsRounded.ttf DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/asset/font)

target_include_directories(jekyll PUBLIC include lib/glad/include lib/bStream lib/glm lib/ImGuiFileDialog lib/libbti/include)
target_link_libraries(jekyll PUBLIC glm j3dultra imgui glfw magic_enum::magic_enum libbti)
