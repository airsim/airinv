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
#include <stdair/stdair_exceptions.hpp>
// Airinv
#include <airinv/AIRINV_Types.hpp>
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

// //////////////////////////////////////////////////////////////////////
/**
 * Build and sell a default segment.
 */
bool testInventoryHelper (const unsigned short iTestFlag,
                          const stdair::Filename_T& iInventoryInputFilename,  
                          const stdair::Filename_T& iScheduleInputFilename,
                          const stdair::Filename_T& iODInputFilename,
                          const stdair::Filename_T& iYieldInputFilename,
                          const bool isBuiltin,
                          const bool isForSchedule) {

  // Output log File
  std::ostringstream oStr;
  oStr << "InventoryTestSuite_" << iTestFlag << ".log";
  const stdair::Filename_T lLogFilename (oStr.str());

  // Set the log parameters
  std::ofstream logOutputFile;
  // Open and clean the log outputfile
  logOutputFile.open (lLogFilename.c_str());
  logOutputFile.clear();
  
  // Initialise the AirInv service object
  const bool lForceMultipleInit = true;
  stdair::BasLogParams lLogParams (stdair::LOG::DEBUG,
                                   logOutputFile,
                                   lForceMultipleInit);

  // Initialise the inventory service
  AIRINV::AIRINV_Master_Service airinvService (lLogParams);
  
  // Parameters for the sale
  std::string lSegmentDateKey;
  stdair::ClassCode_T lClassCode;
  const stdair::PartySize_T lPartySize (2);

  // Check wether or not a (CSV) input file should be read
  if (isBuiltin == true) {

    // Build the default sample BOM tree (filled with inventories) for AirInv
   airinvService.buildSampleBom();

   // Define a specific segment-date key for the sample BOM tree
   lSegmentDateKey = "BA,9,2011-06-10,LHR,SYD";
   lClassCode = "Q";

  } else {

    if (isForSchedule == true) {
      // Build the BOM tree from parsing a schedule file (and O&D list)
      stdair::ScheduleFilePath lScheduleFilePath (iScheduleInputFilename);
      stdair::ODFilePath lODFilePath (iODInputFilename);
      AIRRAC::YieldFilePath lYieldFilePath (iYieldInputFilename);
      airinvService.parseAndLoad (lScheduleFilePath, lODFilePath,
                                  lYieldFilePath);

      // Define a specific segment-date key for the schedule-based inventory
      lSegmentDateKey = "SQ,11,2010-01-15,SIN,BKK";
      lClassCode = "Y";

    } else {

      // Build the BOM tree from parsing an inventory dump file
      AIRINV::InventoryFilePath lInventoryFilePath (iInventoryInputFilename);
      airinvService.parseAndLoad (lInventoryFilePath);

      // Define a specific segment-date key for the inventory parsed file
      //const std::string lSegmentDateKey ("SV, 5, 2010-03-11, KBP, JFK, 08:00:00");
      lSegmentDateKey =  "SV, 5, 2010-03-11, KBP, JFK, 08:00:00";
      lClassCode = "J";
    }

  }

  // Make a booking
  const bool hasSaleBeenSuccessful =
    airinvService.sell (lSegmentDateKey, lClassCode, lPartySize);

  // DEBUG: Display the list of travel solutions
  const std::string& lCSVDump = airinvService.csvDisplay();
  STDAIR_LOG_DEBUG (lCSVDump);

  // Close the log file
  logOutputFile.close();

  if (hasSaleBeenSuccessful == false) {
    STDAIR_LOG_DEBUG ("No sale can be made for '" << lSegmentDateKey
                      << "'");
  }
  
  return hasSaleBeenSuccessful;

}

// /////////////// Main: Unit Test Suite //////////////

// Set the UTF configuration (re-direct the output to a specific file)
BOOST_GLOBAL_FIXTURE (UnitTestConfig);

// Start the test suite
BOOST_AUTO_TEST_SUITE (master_test_suite)

/**
 * Test a simple inventory sell from parsing an inventory input file.
 */
BOOST_AUTO_TEST_CASE (airinv_simple_inventory_sell) {

  // Input file name
  const stdair::Filename_T lInventoryInputFilename (STDAIR_SAMPLE_DIR
                                                    "/invdump01.csv");
  
  // State whether the BOM tree should be built-in or parsed from an input file
  const bool isBuiltin = false;
  // State whether the BOM tree should be built from a schedule file (instead of from an inventory dump)
  const bool isForSchedule = false;

  // Try sell a default segment.
  bool hasTestBeenSuccessful = false;
  BOOST_CHECK_NO_THROW (hasTestBeenSuccessful =
                        testInventoryHelper (0, lInventoryInputFilename,
                                             " ", " ", " ", isBuiltin, isForSchedule));
  BOOST_CHECK_EQUAL (hasTestBeenSuccessful, true);

}

/**
 * Test a simple inventory sell with the default BOM tree.
 */
BOOST_AUTO_TEST_CASE (airinv_simple_inventory_sell_built_in) {
  
  // State whether the BOM tree should be built-in or parsed from an input file
  const bool isBuiltin = true;
  // State whether the BOM tree should be built from a schedule file (instead of from an inventory dump)
  const bool isForSchedule = false;

  // Try sell a default segment.
  bool hasTestBeenSuccessful = false;
  BOOST_CHECK_NO_THROW (hasTestBeenSuccessful =
                        testInventoryHelper (1, " ", " ", " ", " ",
                                             isBuiltin, isForSchedule));
  BOOST_CHECK_EQUAL (hasTestBeenSuccessful, true);

}

/**
 * Test a simple inventory from a schedule file (instead of from an inventory dump).
 */
BOOST_AUTO_TEST_CASE (airinv_simple_inventory_sell_schedule) {

  // Input file names
  const stdair::Filename_T lScheduleInputFilename (STDAIR_SAMPLE_DIR
                                                   "/schedule01.csv");
  const stdair::Filename_T lODInputFilename (STDAIR_SAMPLE_DIR
                                             "/ond01.csv");
  const stdair::Filename_T lYieldInputFilename (STDAIR_SAMPLE_DIR
                                                "/yieldstore01.csv");
  
  // State whether the BOM tree should be built-in or parsed from an input file
  const bool isBuiltin = false;
  // State whether the BOM tree should be built from a schedule file (instead of from an inventory dump)
  const bool isForSchedule = true;

  // Try sell a default segment.
  bool hasTestBeenSuccessful = false;
  BOOST_CHECK_NO_THROW (hasTestBeenSuccessful =
                        testInventoryHelper (2, " ",
                                             lScheduleInputFilename,
                                             lODInputFilename,
                                             lYieldInputFilename,
                                             isBuiltin, isForSchedule));
  BOOST_CHECK_EQUAL (hasTestBeenSuccessful, true);

}

/**
 * Test an error detection functionality: wrong inventory filepath.
 * Expected to throw AIRINV::InventoryInputFileNotFoundException
 */
BOOST_AUTO_TEST_CASE (airinv_error_inventory_input_file) {

  // Inventory input file name
  const stdair::Filename_T lMissingInventoryFilename (STDAIR_SAMPLE_DIR
                                                      "/missingFile.csv");
  
  // State whether the BOM tree should be built-in or parsed from an input file
  const bool isBuiltin = false;
  // State whether the BOM tree should be built from a schedule file (instead of from an inventory dump)
  const bool isForSchedule = false;

  // Try sell a default segment.
  BOOST_CHECK_THROW (testInventoryHelper (3, lMissingInventoryFilename,
                                          " ", " ", " ", isBuiltin, isForSchedule),
                     AIRINV::InventoryInputFileNotFoundException);

}

/**
 * Test an error detection functionality: wrong schedule filepath.
 * Expected to throw AIRINV::ScheduleInputFileNotFoundException
 */
BOOST_AUTO_TEST_CASE (airinv_error_schedule_input_file) {

  // Schedule input file name
  const stdair::Filename_T lMissingScheduleFilename (STDAIR_SAMPLE_DIR
                                                     "/missingFile.csv");
  
  // State whether the BOM tree should be built-in or parsed from an input file
  const bool isBuiltin = false;
  // State whether the BOM tree should be built from a schedule file (instead of from an inventory dump)
  const bool isForSchedule = true;

  // Try sell a default segment.
  BOOST_CHECK_THROW (testInventoryHelper (4, " ", lMissingScheduleFilename,
                                          " ", " ", isBuiltin, isForSchedule),
                     AIRINV::ScheduleInputFileNotFoundException);

}

/**
 * Test an error detection functionality: wrong yield filepath.
 * Expected to throw AIRRAC::YieldInputFileNotFoundException
 */
BOOST_AUTO_TEST_CASE (airinv_error_yield_input_file) {
  
  // Input file names
  const stdair::Filename_T lScheduleInputFilename (STDAIR_SAMPLE_DIR
                                                   "/schedule01.csv");
  const stdair::Filename_T lODInputFilename (STDAIR_SAMPLE_DIR
                                             "/ond01.csv");
  const stdair::Filename_T lYieldInputFilename (STDAIR_SAMPLE_DIR
                                                "/missingFile.csv");
  
  // State whether the BOM tree should be built-in or parsed from an input file
  const bool isBuiltin = false;
  // State whether the BOM tree should be built from a schedule file (instead of from an inventory dump)
  const bool isForSchedule = true;

  // Try sell a default segment.
  BOOST_CHECK_THROW (testInventoryHelper (5, " ",
                                          lScheduleInputFilename,
                                          lODInputFilename,
                                          lYieldInputFilename,
                                          isBuiltin, isForSchedule),
                     AIRRAC::YieldInputFileNotFoundException);

}

/**
 * Test an error detection functionality: wrong yield filepath.
 * Expected to throw AIRRAC::YieldInputFileNotFoundException
 */
BOOST_AUTO_TEST_CASE (airinv_error_flight_date_duplication) {
  
  // Input file names
  const stdair::Filename_T lScheduleInputFilename (STDAIR_SAMPLE_DIR
                                                   "/scheduleError01.csv");
  const stdair::Filename_T lODInputFilename (STDAIR_SAMPLE_DIR
                                             "/ond01.csv");
  const stdair::Filename_T lYieldInputFilename (STDAIR_SAMPLE_DIR
                                                "/missingFile.csv");
  
  // State whether the BOM tree should be built-in or parsed from an input file
  const bool isBuiltin = false;
  // State whether the BOM tree should be built from a schedule file (instead of from an inventory dump)
  const bool isForSchedule = true;

  // Try sell a default segment.
  BOOST_CHECK_THROW (testInventoryHelper (6, " ",
                                          lScheduleInputFilename,
                                          lODInputFilename,
                                          lYieldInputFilename,
                                          isBuiltin, isForSchedule),
                     AIRINV::FlightDateDuplicationException);

}

/**
 * Test an error detection functionality: parsing of schedule input file failed.
 * Expected to throw AIRINV::ScheduleFileParsingFailedException
 */
BOOST_AUTO_TEST_CASE (airinv_error_schedule_parsing_failed) {
  
  // Input file names
  const stdair::Filename_T lScheduleInputFilename (STDAIR_SAMPLE_DIR
                                                   "/scheduleError02.csv");
  const stdair::Filename_T lODInputFilename (STDAIR_SAMPLE_DIR
                                             "/ond01.csv");
  const stdair::Filename_T lYieldInputFilename (STDAIR_SAMPLE_DIR
                                                "/yieldstore01.csv");
  
  // State whether the BOM tree should be built-in or parsed from an input file
  const bool isBuiltin = false;
  // State whether the BOM tree should be built from a schedule file (instead of from an inventory dump)
  const bool isForSchedule = true;

  // Try sell a default segment.
  BOOST_CHECK_THROW (testInventoryHelper (7, " ",
                                          lScheduleInputFilename,
                                          lODInputFilename,
                                          lYieldInputFilename,
                                          isBuiltin, isForSchedule),
                     AIRINV::ScheduleFileParsingFailedException);

}

// End the test suite
BOOST_AUTO_TEST_SUITE_END()

/*!
 * \endcode
 */

