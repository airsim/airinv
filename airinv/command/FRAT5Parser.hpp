#ifndef __AIRINV_CMD_FRAT5PARSER_HPP
#define __AIRINV_CMD_FRAT5PARSER_HPP

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
  class FRAT5Parser : public stdair::CmdAbstract {
  public:
    /**
     * Parse the CSV file describing the FRAT5 curves for the
     * simulator, and generates the curves accordingly.
     *
     * @param const stdair::Filename_T& The file-name of the CSV-formatted 
     *        FRAT5 curve input file.
     * @param stdair::BomRoot& Root of the BOM tree.
     */
    static void parse (const stdair::FRAT5FilePath& iFRAT5InputFilename,
                       stdair::BomRoot&);
  };
}
#endif // __AIRINV_CMD_FRAT5PARSER_HPP
