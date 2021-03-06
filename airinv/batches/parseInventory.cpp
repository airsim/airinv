/*!
 * \page batch_airinv_cpp Command-Line Utility to Demonstrate Typical AirInv Usage
 * \code
 */
// STL
#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
// Boost (Extended STL)
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
// StdAir
#include <stdair/basic/BasLogParams.hpp>
#include <stdair/basic/BasDBParams.hpp>
#include <stdair/service/Logger.hpp>
// AirInv
#include <airinv/AIRINV_Master_Service.hpp>
#include <airinv/config/airinv-paths.hpp>

// //////// Constants //////
/**
 * Default name and location for the log file.
 */
const std::string K_AIRINV_DEFAULT_LOG_FILENAME ("parseInventory.log");

/**
 * Default name and location for the (CSV) input files.
 */
const std::string K_AIRINV_DEFAULT_INVENTORY_FILENAME (STDAIR_SAMPLE_DIR
                                                       "/invdump01.csv");
/**
 * Default name and location for the (CSV) input files.
 */
const std::string K_AIRINV_DEFAULT_SCHEDULE_FILENAME (STDAIR_SAMPLE_DIR
                                                      "/schedule01.csv");
/**
 * Default name and location for the (CSV) input files.
 */
const std::string K_AIRINV_DEFAULT_OND_FILENAME (STDAIR_SAMPLE_DIR
                                                 "/ond01.csv");

/**
 * Default name and location for the (CSV) input files.
 */
const std::string K_AIRINV_DEFAULT_YIELD_FILENAME (STDAIR_SAMPLE_DIR
                                                   "/yieldstore01.csv");

/**
 * Default segment-date key on which the sale should be made.
 */
const std::string K_AIRINV_DEFAULT_SEGMENT_DATE_KEY ("SV,5,2010-03-11,KBP,JFK");

/**
 * Default class code for which the sale should be made.
 */
const stdair::ClassCode_T K_AIRINV_DEFAULT_CLASS_CODE ("Y");

/**
 * Default party size for the sale.
 */
const stdair::PartySize_T K_AIRINV_DEFAULT_PARTY_SIZE (2);

/**
 * Default for the input type. It can be either built-in or provided by an
 * input file. That latter must then be given with the -i option.
 */
const bool K_AIRINV_DEFAULT_BUILT_IN_INPUT = false;

/**
 * Default for the input type. The BOM tree can be built from either an
 * inventory dump or from a schedule file (and, potentially, an O&D list).
 */
const bool K_AIRINV_DEFAULT_FOR_SCHEDULE = false;

/**
 * Early return status (so that it can be differentiated from an error).
 */
const int K_AIRINV_EARLY_RETURN_STATUS = 99;

// ///////// Parsing of Options & Configuration /////////
// A helper function to simplify the main part.
template<class T> std::ostream& operator<< (std::ostream& os,
                                            const std::vector<T>& v) {
  std::copy (v.begin(), v.end(), std::ostream_iterator<T> (std::cout, " ")); 
  return os;
}

/**
 * Read and parse the command line options.
 */
int readConfiguration (int argc, char* argv[],
                       bool& ioIsBuiltin, bool& ioIsForSchedule,
                       stdair::Filename_T& ioInventoryFilename,
                       stdair::Filename_T& ioScheduleInputFilename,
                       stdair::Filename_T& ioODInputFilename,
                       stdair::Filename_T& ioYieldInputFilename,
                       std::string& ioSegmentDateKey,
                       stdair::ClassCode_T& ioClassCode,
                       stdair::PartySize_T& ioPartySize,
                       std::string& ioLogFilename) {
  // Default for the built-in input
  ioIsBuiltin = K_AIRINV_DEFAULT_BUILT_IN_INPUT;

  // Default for the inventory or schedule option
  ioIsForSchedule = K_AIRINV_DEFAULT_FOR_SCHEDULE;

  // Declare a group of options that will be allowed only on command line
  boost::program_options::options_description generic ("Generic options");
  generic.add_options()
    ("prefix", "print installation prefix")
    ("version,v", "print version string")
    ("help,h", "produce help message");
    
  // Declare a group of options that will be allowed both on command
  // line and in config file

  boost::program_options::options_description config ("Configuration");
  config.add_options()
    ("builtin,b",
     "The sample BOM tree can be either built-in or parsed from an input file. That latter must then be given with the -i/--inventory or -s/--schedule option")
    ("for_schedule,f",
     "The BOM tree should be built from a schedule file (instead of from an inventory dump)")
    ("inventory,i",
     boost::program_options::value< std::string >(&ioInventoryFilename)->default_value(K_AIRINV_DEFAULT_INVENTORY_FILENAME),
     "(CSV) input file for the inventory")
    ("schedule,s",
     boost::program_options::value< std::string >(&ioScheduleInputFilename)->default_value(K_AIRINV_DEFAULT_SCHEDULE_FILENAME),
     "(CSV) input file for the schedule")
    ("ond,o",
     boost::program_options::value< std::string >(&ioODInputFilename)->default_value(K_AIRINV_DEFAULT_OND_FILENAME),
     "(CSV) input file for the O&D")
    ("yield,y",
     boost::program_options::value< std::string >(&ioYieldInputFilename)->default_value(K_AIRINV_DEFAULT_YIELD_FILENAME),
     "(CSV) input file for the yield")
    ("segment_date_key,k",
     boost::program_options::value< std::string >(&ioSegmentDateKey)->default_value(K_AIRINV_DEFAULT_SEGMENT_DATE_KEY),
     "Segment-date key")
    ("class_code,c",
     boost::program_options::value< stdair::ClassCode_T >(&ioClassCode)->default_value(K_AIRINV_DEFAULT_CLASS_CODE),
     "Class code")
    ("party_size,p",
     boost::program_options::value< stdair::PartySize_T >(&ioPartySize)->default_value(K_AIRINV_DEFAULT_PARTY_SIZE),
     "Party size")
    ("log,l",
     boost::program_options::value< std::string >(&ioLogFilename)->default_value(K_AIRINV_DEFAULT_LOG_FILENAME),
     "Filename for the logs")
    ;

  // Hidden options, will be allowed both on command line and
  // in config file, but will not be shown to the user.
  boost::program_options::options_description hidden ("Hidden options");
  hidden.add_options()
    ("copyright",
     boost::program_options::value< std::vector<std::string> >(),
     "Show the copyright (license)");
        
  boost::program_options::options_description cmdline_options;
  cmdline_options.add(generic).add(config).add(hidden);

  boost::program_options::options_description config_file_options;
  config_file_options.add(config).add(hidden);
  boost::program_options::options_description visible ("Allowed options");
  visible.add(generic).add(config);
        
  boost::program_options::positional_options_description p;
  p.add ("copyright", -1);
        
  boost::program_options::variables_map vm;
  boost::program_options::
    store (boost::program_options::command_line_parser (argc, argv).
           options (cmdline_options).positional(p).run(), vm);

  std::ifstream ifs ("airinv.cfg");
  boost::program_options::store (parse_config_file (ifs, config_file_options),
                                 vm);
  boost::program_options::notify (vm);
    
  if (vm.count ("help")) {
    std::cout << visible << std::endl;
    return K_AIRINV_EARLY_RETURN_STATUS;
  }

  if (vm.count ("version")) {
    std::cout << PACKAGE_NAME << ", version " << PACKAGE_VERSION << std::endl;
    return K_AIRINV_EARLY_RETURN_STATUS;
  }

  if (vm.count ("prefix")) {
    std::cout << "Installation prefix: " << PREFIXDIR << std::endl;
    return K_AIRINV_EARLY_RETURN_STATUS;
  }

  if (vm.count ("builtin")) {
    ioIsBuiltin = true;
  }
  const std::string isBuiltinStr = (ioIsBuiltin == true)?"yes":"no";
  std::cout << "The BOM should be built-in? " << isBuiltinStr << std::endl;

  if (vm.count ("for_schedule")) {
    ioIsForSchedule = true;
  }
  const std::string isForScheduleStr = (ioIsForSchedule == true)?"yes":"no";
  std::cout << "The BOM should be built from schedule? " << isForScheduleStr
            << std::endl;

  if (ioIsBuiltin == false) {

    if (ioIsForSchedule == false) {
      // The BOM tree should be built from parsing an inventory dump
      if (vm.count ("inventory")) {
        ioInventoryFilename = vm["inventory"].as< std::string >();
        std::cout << "Input inventory filename is: " << ioInventoryFilename
                  << std::endl;

      } else {
        // The built-in option is not selected. However, no inventory dump
        // file is specified
        std::cerr << "Either one among the -b/--builtin, -i/--inventory or "
                  << " -f/--for_schedule and -s/--schedule options "
                  << "must be specified" << std::endl;
      }

    } else {
      // The BOM tree should be built from parsing a schedule (and O&D) file
      if (vm.count ("schedule")) {
        ioScheduleInputFilename = vm["schedule"].as< std::string >();
        std::cout << "Input schedule filename is: " << ioScheduleInputFilename
                  << std::endl;

      } else {
        // The built-in option is not selected. However, no schedule file
        // is specified
        std::cerr << "Either one among the -b/--builtin, -i/--inventory or "
                  << " -f/--for_schedule and -s/--schedule options "
                  << "must be specified" << std::endl;
      }

      if (vm.count ("ond")) {
        ioODInputFilename = vm["ond"].as< std::string >();
        std::cout << "Input O&D filename is: " << ioODInputFilename << std::endl;
      }

      if (vm.count ("yield")) {
        ioYieldInputFilename = vm["yield"].as< std::string >();
        std::cout << "Input yield filename is: "
                  << ioYieldInputFilename << std::endl;
      }
    }
  }

  if (vm.count ("log")) {
    ioLogFilename = vm["log"].as< std::string >();
    std::cout << "Log filename is: " << ioLogFilename << std::endl;
  }

  return 0;
}


// ///////// M A I N ////////////
int main (int argc, char* argv[]) {

  // State whether the BOM tree should be built-in or parsed from an
  // input file
  bool isBuiltin;
  bool isForSchedule;

  // Input file names
  stdair::Filename_T lInventoryFilename;
  stdair::Filename_T lScheduleInputFilename;
  stdair::Filename_T lODInputFilename;
  stdair::Filename_T lYieldInputFilename;

  // Parameters for the sale
  std::string lSegmentDateKey;
  stdair::ClassCode_T lClassCode;
  stdair::PartySize_T lPartySize;

  // Output log File
  stdair::Filename_T lLogFilename;

  // Call the command-line option parser
  const int lOptionParserStatus =
    readConfiguration (argc, argv, isBuiltin, isForSchedule, lInventoryFilename,
                       lScheduleInputFilename, lODInputFilename,
                       lYieldInputFilename, lSegmentDateKey, lClassCode,
                       lPartySize, lLogFilename);

  if (lOptionParserStatus == K_AIRINV_EARLY_RETURN_STATUS) {
    return 0;
  }

  // Set the log parameters
  std::ofstream logOutputFile;
  // Open and clean the log outputfile
  logOutputFile.open (lLogFilename.c_str());
  logOutputFile.clear();

  // Initialise the inventory service
  const stdair::BasLogParams lLogParams (stdair::LOG::DEBUG, logOutputFile);
  AIRINV::AIRINV_Master_Service airinvService (lLogParams);

  // DEBUG
  STDAIR_LOG_DEBUG ("Welcome to AirInv");

  // Check wether or not a (CSV) input file should be read
  if (isBuiltin == true) {

    // Build the sample BOM tree for RMOL
    airinvService.buildSampleBom();

    // Define a specific segment-date key for the sample BOM tree
    //lSegmentDateKey = "BA,9,2011-06-10,LHR,SYD";
    lSegmentDateKey = "SQ,11,2010-02-08,SIN,BKK";

  } else {
    if (isForSchedule == true) {
      // Build the BOM tree from parsing a schedule file (and O&D list)
      stdair::ScheduleFilePath lScheduleFilePath (lScheduleInputFilename);
      stdair::ODFilePath lODFilePath (lODInputFilename);
      AIRRAC::YieldFilePath lYieldFilePath (lYieldInputFilename);
      airinvService.parseAndLoad (lScheduleFilePath, lODFilePath,
                                  lYieldFilePath);

      if (lSegmentDateKey == K_AIRINV_DEFAULT_SEGMENT_DATE_KEY) {
        // Define a specific segment-date key for the schedule-based inventory
        lSegmentDateKey = "SQ,11,2010-01-15,SIN,BKK";
      }

    } else {
      // Build the BOM tree from parsing an inventory dump file
      AIRINV::InventoryFilePath lInventoryFilePath (lInventoryFilename);
      airinvService.parseAndLoad (lInventoryFilePath);
    }
  }

  // Make a booking
  const bool isSellSuccessful =
    airinvService.sell (lSegmentDateKey, lClassCode, lPartySize);

  // DEBUG
  STDAIR_LOG_DEBUG ("Sale ('" << lSegmentDateKey << "', " << lClassCode << ": "
                    << lPartySize << ") successful? " << isSellSuccessful);

  // DEBUG: Display the whole BOM tree
  const std::string& lCSVDump = airinvService.csvDisplay();
  STDAIR_LOG_DEBUG (lCSVDump);

  // Close the Log outputFile
  logOutputFile.close();

  /*
    Note: as that program is not intended to be run on a server in
    production, it is better not to catch the exceptions. When it
    happens (that an exception is throwned), that way we get the
    call stack.
  */

  return 0;	
}
