// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <sstream>
// StdAir
#include <stdair/basic/BasFileMgr.hpp>
#include <stdair/bom/BomRoot.hpp>
#include <stdair/service/Logger.hpp>
// Airinv
#include <airinv/command/ScheduleParserHelper.hpp>
#include <airinv/command/ScheduleParser.hpp>
#include <airinv/command/InventoryManager.hpp>

namespace AIRINV {

  // //////////////////////////////////////////////////////////////////////
  void ScheduleParser::
  generateInventories (const stdair::ScheduleFilePath& iScheduleFilename,
                       stdair::BomRoot& ioBomRoot) {

    const stdair::Filename_T lFilename = iScheduleFilename.name();

    // Check that the file path given as input corresponds to an actual file
    bool doesExistAndIsReadable =
      stdair::BasFileMgr::doesExistAndIsReadable (lFilename);
    if (doesExistAndIsReadable == false) {
      std::ostringstream oMessage;
      oMessage << "The schedule input file, '" << lFilename
               << "', can not be retrieved on the file-system";
      STDAIR_LOG_ERROR (oMessage.str());
      throw ScheduleInputFileNotFoundException (oMessage.str());
    }

    // Initialise the Flight-Period file parser.
    FlightPeriodFileParser lFlightPeriodParser (ioBomRoot, lFilename);

    // Parse the CSV-formatted schedule input file, and generate the
    // corresponding Inventories for the airlines.
    lFlightPeriodParser.generateInventories ();

  }

}
