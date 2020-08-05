add_clang_executable(clang-function-prototype FindFunction.cpp)

target_link_libraries(clang-function-prototype PRIVATE clangTooling)
