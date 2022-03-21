    cmake_minimum_required(VERSION 3.13)

    # Ustaw nazwę projektu.
    project(net_simulation)

    # Ustaw standard C++ (tu: C++17).
    set(CMAKE_CXX_STANDARD 17)

    # Dodaj flagi kompilacji wymuszające pisanie kodu zgodnego ze standardem.
    add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic-errors -Werror=switch)

    # Dodaj katalogi z plikami nagłówkowymi dla wszystkich konfiguracji.
    include_directories(
            include
    )

    # Ustaw zmienną `SOURCE_FILES`, która będzie przechowywać ścieżki do
    # tych plików źródłowych, które będą niezbędne w każdej konfiguracji.
    set(SOURCE_FILES
            src/package.cpp
            src/storage_types.cpp
            src/nodes.cpp
            src/helpers.cpp
            src/factory.cpp
            src/reports.cpp
            src/simulation.cpp)

    # Dodaj konfigurację typu `Debug`.
    add_executable(net_simulation_debug ${SOURCE_FILES} main.cpp)


    # == Unit testing using Google Testing Framework ==

    # Ustaw zmienną `SOURCES_FILES_TESTS`, która będzie przechowywać ścieżki do
    # plików źródłowych z testami jednostkowymi.
    set(SOURCES_FILES_TESTS
            test/test_package.cpp
            test/test_storage_types.cpp
            test/test_nodes.cpp;
            test/test_Factory.cpp
            test/test_factory_io.cpp
            test/test_reports.cpp
            test/test_simulate.cpp
            )

    # Dodaj konfigurację typu `Test`.
    add_executable(net_simulation_test ${SOURCE_FILES} ${SOURCES_FILES_TESTS} test/main_gtest.cpp)

    # Dodaj katalogi z plikami nagłówkowymi wyłącznie do konkretnej konfiguracji (tu: `Test`).
    target_include_directories(net_simulation_test PUBLIC
            googletest-master/googlemock/include
            googletest-master/googletest/include
            mocks
            )

    target_compile_definitions(net_simulation_test PUBLIC EXERCISE_ID=EXERCISE_ID_FACTORY)

    # Dodaj podkatalog `googletest-master`.
    add_subdirectory(googletest-master)

    # Podlinkuj bibliotekę o identyfikatorze `gmock` (w pliku CMake) wyłącznie do konkretnej
    # konfiguracji (tu: `Test`).
    target_link_libraries(net_simulation_test gmock)

