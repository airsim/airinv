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
#include <airinv/command/FFDisutilityParserHelper.hpp>
#include <airinv/command/FFDisutilityParser.hpp>
#include <airinv/command/InventoryManager.hpp>

namespace AIRINV {

  // //////////////////////////////////////////////////////////////////////
  void FFDisutilityParser::
  parse (const stdair::FFDisutilityFilePath& iFFDisutilityFilename,
         stdair::BomRoot& ioBomRoot) {

    const stdair::Filename_T lFilename = iFFDisutilityFilename.name();

    // Check that the file path given as input corresponds to an actual file
    bool doesExistAndIsReadable =
      stdair::BasFileMgr::doesExistAndIsReadable (lFilename);
    if (doesExistAndIsReadable == false) {
      std::ostringstream oMessage;
      oMessage << "The FF disutility input file, '" << lFilename
               << "', can not be retrieved on the file-system";
      STDAIR_LOG_ERROR (oMessage.str());
      throw FFDisutilityInputFileNotFoundException (oMessage.str());
    }

    // Initialise the FFDisutility file parser.
    FFDisutilityFileParser lFFDisutilityParser (ioBomRoot, lFilename);

    // Parse the CSV-formatted FFDisutility input file, and generate the
    // corresponding FFDisutility curves.
    lFFDisutilityParser.generateFFDisutilityCurves ();
  }
}
