add_ryu_test(current_time_test current_time_test.cpp)

# Also tested for coverage in connection_test.cpp
# but full testing requires some private APIs
add_ryu_test(transaction_test transaction_test.cpp checkpoint_test.cpp wal_test.cpp)
