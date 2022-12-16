add_executable(zero-bond-yield zero-bond-yield.cpp)
target_link_libraries(zero-bond-yield ql_library ${QL_THREAD_LIBRARIES})
if (QL_INSTALL_EXAMPLES)
    install(TARGETS zero-bond-yield RUNTIME DESTINATION ${QL_INSTALL_EXAMPLESDIR})
endif()

