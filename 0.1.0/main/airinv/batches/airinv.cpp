// STL
#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
// StdAir
#include <stdair/STDAIR_Types.hpp>
#include <stdair/basic/BasLogParams.hpp>
#include <stdair/basic/BasDBParams.hpp>
#include <stdair/bom/TravelSolutionStruct.hpp>
// AirInv
#include <airinv/AIRINV_Service.hpp>
#include <airinv/config/airinv-paths.hpp>

// ///////// M A I N ////////////
int main (int argc, char* argv[]) {

  try {
    
    // Airline code
    const std::string lAirlineCode ("SV");
    
    // Number of passengers in the travelling group
    // const stdair::PartySize_T lPartySize = 5;

    // Travel solution
    // stdair::TravelSolutionStruct lTravelSolution;
    
    // Input file name
    stdair::Filename_T lInputFilename ("../../test/samples/invdump01.csv");

    // Output log File
    stdair::Filename_T lLogFilename ("airinv.log");

    // Set the log parameters
    std::ofstream logOutputFile;
    // Open and clean the log outputfile
    logOutputFile.open (lLogFilename.c_str());
    logOutputFile.clear();
    
    // Initialise the list of classes/buckets
    const stdair::BasLogParams lLogParams (stdair::LOG::DEBUG, logOutputFile);
    AIRINV::AIRINV_Service airinvService (lLogParams, lAirlineCode,
                                          lInputFilename);

    // Make a booking
    // airinvService.sell (lTravelSolution, lPartySize);
    
  } catch (const std::exception& stde) {
    std::cerr << "Standard exception: " << stde.what() << std::endl;
    return -1;
    
  } catch (...) {
    return -1;
  }
  
  return 0;	
}