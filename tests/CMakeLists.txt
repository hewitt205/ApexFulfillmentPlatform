add_executable(apex_tests
    test_coordination.cpp
    test_domain.cpp
)
target_include_directories(apex_tests PRIVATE ${CMAKE_SOURCE_DIR}/src .)
target_link_libraries(apex_tests PRIVATE apex_coordination)

add_test(NAME apex_tests COMMAND apex_tests)
