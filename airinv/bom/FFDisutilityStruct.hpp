#ifndef __AIRINV_BOM_FFDISUTILITYSTRUCT_HPP
#define __AIRINV_BOM_FFDISUTILITYSTRUCT_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <string>
// StdAir
#include <stdair/stdair_rm_types.hpp>

namespace AIRINV {

  /** Utility Structure for the parsing of FFDisutility structures. */
  struct FFDisutilityStruct : public stdair::StructAbstract {
    
  public:
    // ////////////////// Display Support Methods ////////////////
    /** Give a description of the structure (for display purposes). */
    const std::string describe() const;


  public:
    // /////////////// Constructors and destructors ///////////////
    /** Default constructor. */
    FFDisutilityStruct();
    /** Destructor */
    ~FFDisutilityStruct();
  private:
    /** Deault copy constructor. */
    FFDisutilityStruct (const FFDisutilityStruct&);


  public:
    // ////////////// Attributes ///////////////////
    /** Curve key. */
    std::string _key;

    /** Curve. */
    stdair::FFDisutilityCurve_T _curve;
    
  public:
    // ///////////////// Staging ///////////////////
    /** DTD. */
    stdair::DTD_T _dtd;
  };

}
#endif // __AIRINV_BOM_FFDISUTILITYSTRUCT_HPP
