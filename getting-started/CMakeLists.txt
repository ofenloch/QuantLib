add_executable(zero-coupon-bond-yield zero-coupon-bond-yield.cpp)
target_link_libraries(zero-coupon-bond-yield ql_library ${QL_THREAD_LIBRARIES})

add_executable(time-series time-series.cpp)
target_link_libraries(time-series ql_library ${QL_THREAD_LIBRARIES})

add_executable(fixed-rate-bond-yield fixed-rate-bond-yield.cpp)
target_link_libraries(fixed-rate-bond-yield ql_library ${QL_THREAD_LIBRARIES})
