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
#include <airinv/command/FRAT5ParserHelper.hpp>
#include <airinv/command/FRAT5Parser.hpp>
#include <airinv/command/InventoryManager.hpp>

namespace AIRINV {

  // //////////////////////////////////////////////////////////////////////
  void FRAT5Parser::
  parse (const stdair::FRAT5FilePath& iFRAT5Filename,
         stdair::BomRoot& ioBomRoot) {

    const stdair::Filename_T lFilename = iFRAT5Filename.name();

    // Check that the file path given as input corresponds to an actual file
    bool doesExistAndIsReadable =
      stdair::BasFileMgr::doesExistAndIsReadable (lFilename);
    if (doesExistAndIsReadable == false) {
      std::ostringstream oMessage;
      oMessage << "The FRAT5 input file, '" << lFilename
               << "', can not be retrieved on the file-system";
      STDAIR_LOG_ERROR (oMessage.str());
      throw FRAT5InputFileNotFoundException (oMessage.str());
    }

    // Initialise the FRAT5 file parser.
    FRAT5FileParser lFRAT5Parser (ioBomRoot, lFilename);

    // Parse the CSV-formatted FRAT5 input file, and generate the
    // corresponding FRAT5 curves.
    lFRAT5Parser.generateFRAT5Curves ();
  }
}
