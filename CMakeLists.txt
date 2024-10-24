# CMakeLists.txt
cmake_minimum_required(VERSION 3.14)
project(olilands_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Encontrar Boost
find_package(Boost REQUIRED COMPONENTS system)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
endif()

# Usar FetchContent para baixar spdlog
include(FetchContent)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.14.1
)
FetchContent_MakeAvailable(spdlog)

# Buscar todos os arquivos .cpp recursivamente na pasta src/
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/**/*.cpp")

# Adicionar executável antes de definir include directories
add_executable(olilands_server ${SOURCES})

# Definir diretórios de inclusão
target_include_directories(olilands_server PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/src
)

# Linkar bibliotecas Boost e spdlog
target_link_libraries(olilands_server PRIVATE
        Boost::system
        spdlog::spdlog
)
