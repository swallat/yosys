add_executable(rtlilTest ${CMAKE_CURRENT_LIST_DIR}/kernel/rtlilTest.cc)
target_link_libraries(rtlilTest GTest::GTest GTest::Main)
gtest_add_tests(rtlilTest "" AUTO)

add_executable(logTest ${CMAKE_CURRENT_LIST_DIR}/kernel/logTest.cc)
target_link_libraries(logTest GTest::GTest GTest::Main)
gtest_add_tests(logTest "" AUTO)
