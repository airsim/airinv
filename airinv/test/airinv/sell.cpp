// STL
#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
// StdAir
#include <stdair/STDAIR_Types.hpp>
// AirInv
#include <airinv/AIRINV_Service.hpp>
#include <airinv/config/airinv-paths.hpp>

// ///////// M A I N ////////////
int main (int argc, char* argv[]) {

  try {
    
    // Airline code
    std::string lAirlineCode ("LH");
    
    // Number of passengers in the travelling group
    stdair::PartySize_T lPartySize = 5;
    
    // Output log File
    std::string lLogFilename ("sell.log");

    // Set the log parameters
    std::ofstream logOutputFile;
    // Open and clean the log outputfile
    logOutputFile.open (lLogFilename.c_str());
    logOutputFile.clear();
    
    // Initialise the list of classes/buckets
    const stdair::BasLogParams lLogParams (stdair::LOG::DEBUG, logOutputFile);
    AIRINV::AIRINV_Service airinvService (lLogParams, lAirlineCode);

    // Make a booking
    airinvService.sell (lPartySize);
    
  } catch (const std::exception& stde) {
    std::cerr << "Standard exception: " << stde.what() << std::endl;
    return -1;
    
  } catch (...) {
    return -1;
  }
  
  return 0;	
}
