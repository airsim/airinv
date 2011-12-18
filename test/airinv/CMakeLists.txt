#
# AirInv Tests
#

##
# Specify each test suite configuration

# * InventoryTestSuite Test Suite
module_test_add_suite (airinv InventoryTestSuite InventoryTestSuite.cpp)


##
# Register all the test suites to be built and performed
module_test_build_all ()

