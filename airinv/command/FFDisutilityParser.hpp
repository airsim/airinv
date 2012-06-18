#ifndef __AIRINV_CMD_FFDISUTILITYPARSER_HPP
#define __AIRINV_CMD_FFDISUTILITYPARSER_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// StdAir
#include <stdair/stdair_basic_types.hpp>
#include <stdair/stdair_file.hpp>
#include <stdair/command/CmdAbstract.hpp>

/// Forward declarations.
namespace stdair {
  class BomRoot;
}

namespace AIRINV {
  
  /**
   * @brief Class wrapping the parser entry point.
   */
  class FFDisutilityParser : public stdair::CmdAbstract {
  public:
    /**
     * Parse the CSV file describing the FFDisutility curves for the
     * simulator, and generates the curves accordingly.
     *
     * @param const stdair::Filename_T& The file-name of the CSV-formatted 
     *        FFDisutility curve input file.
     * @param stdair::BomRoot& Root of the BOM tree.
     */
    static void parse (const stdair::FFDisutilityFilePath& iFFDisutilityInputFilename,
                       stdair::BomRoot&);
  };
}
#endif // __AIRINV_CMD_FFDISUTILITYPARSER_HPP
