#
# AirInv Tests
#
module_set_name (airinv)


##
# Specify each test suite configuration

# * InventoryTestSuite Test Suite
module_test_add_suite (InventoryTestSuite InventoryTestSuite.cpp)


##
# Register all the test suites to be built and performed
module_test_build_all ()
