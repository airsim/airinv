/*!
 * \page InventoryTestSuite_cpp Command-Line Test to Demonstrate How To Test the AirInv Project
 * \code
 */
// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <sstream>
#include <fstream>
#include <string>
// Boost Unit Test Framework (UTF)
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE InventoryTestSuite
#include <boost/test/unit_test.hpp>
// StdAir
#include <stdair/basic/BasLogParams.hpp>
#include <stdair/basic/BasDBParams.hpp>
#include <stdair/basic/BasFileMgr.hpp>
#include <stdair/bom/TravelSolutionStruct.hpp>
#include <stdair/bom/BookingRequestStruct.hpp>
#include <stdair/service/Logger.hpp>
// Airinv
//#include <airinv/AIRINV_Types.hpp>
#include <airinv/AIRINV_Master_Service.hpp>
#include <airinv/config/airinv-paths.hpp>

namespace boost_utf = boost::unit_test;

// (Boost) Unit Test XML Report
std::ofstream utfReportStream ("InventoryTestSuite_utfresults.xml");

/**
 * Configuration for the Boost Unit Test Framework (UTF)
 */
struct UnitTestConfig {
  /** Constructor. */
  UnitTestConfig() {
    boost_utf::unit_test_log.set_stream (utfReportStream);
    boost_utf::unit_test_log.set_format (boost_utf::XML);
    boost_utf::unit_test_log.set_threshold_level (boost_utf::log_test_units);
    //boost_utf::unit_test_log.set_threshold_level (boost_utf::log_successful_tests);
  }

  /** Destructor. */
  ~UnitTestConfig() {
  }
};


// /////////////// Main: Unit Test Suite //////////////

// Set the UTF configuration (re-direct the output to a specific file)
BOOST_GLOBAL_FIXTURE (UnitTestConfig);

// Start the test suite
BOOST_AUTO_TEST_SUITE (master_test_suite)

/**
 * Test a simple inventory sell
 */
BOOST_AUTO_TEST_CASE (airinv_simple_inventory_sell) {

  // Input file name
  const stdair::Filename_T lInventoryInputFilename (STDAIR_SAMPLE_DIR
                                                    "/invdump01.csv");

  // Check that the file path given as input corresponds to an actual file
  bool doesExistAndIsReadable =
    stdair::BasFileMgr::doesExistAndIsReadable (lInventoryInputFilename);
  BOOST_CHECK_MESSAGE (doesExistAndIsReadable == true,
                       "The '" << lInventoryInputFilename
                       << "' input file can not be open and read");

  // Output log File
  const stdair::Filename_T lLogFilename ("InventoryTestSuite.log");

  // Set the log parameters
  std::ofstream logOutputFile;
  // Open and clean the log outputfile
  logOutputFile.open (lLogFilename.c_str());
  logOutputFile.clear();

  // Initialise the list of classes/buckets
  const stdair::BasLogParams lLogParams (stdair::LOG::DEBUG, logOutputFile);
  AIRINV::AIRINV_Master_Service airinvService (lLogParams,
                                               lInventoryInputFilename);

  // Make a booking
  const std::string lSegmentDateKey ("SV, 5, 2010-03-11, KBP, JFK, 08:00:00");
  const stdair::ClassCode_T lClassCode ("J");
  const stdair::PartySize_T lPartySize (2);
  const bool hasSaleBeenSuccessful =
    airinvService.sell (lSegmentDateKey, lClassCode, lPartySize);

  // DEBUG: Display the list of travel solutions
  const std::string& lCSVDump = airinvService.csvDisplay();
  STDAIR_LOG_DEBUG (lCSVDump);

  BOOST_CHECK_EQUAL (hasSaleBeenSuccessful, true);
  BOOST_CHECK_MESSAGE (hasSaleBeenSuccessful == true,
                       "No sale can be made for '" << lSegmentDateKey
                       << "'");

  // Close the log file
  logOutputFile.close();
}

// End the test suite
BOOST_AUTO_TEST_SUITE_END()

/*!
 * \endcode
 */

