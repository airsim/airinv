#ifndef __AIRINV_BOM_FRAT5STRUCT_HPP
#define __AIRINV_BOM_FRAT5STRUCT_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <string>
// StdAir
#include <stdair/stdair_rm_types.hpp>

namespace AIRINV {

  /** Utility Structure for the parsing of FRAT5 structures. */
  struct FRAT5Struct : public stdair::StructAbstract {
    
  public:
    // ////////////////// Display Support Methods ////////////////
    /** Give a description of the structure (for display purposes). */
    const std::string describe() const;


  public:
    // /////////////// Constructors and destructors ///////////////
    /** Default constructor. */
    FRAT5Struct();
    /** Destructor */
    ~FRAT5Struct();
  private:
    /** Deault copy constructor. */
    FRAT5Struct (const FRAT5Struct&);


  public:
    // ////////////// Attributes ///////////////////
    /** Curve key. */
    std::string _key;

    /** Curve. */
    stdair::FRAT5Curve_T _curve;
    
  public:
    // ///////////////// Staging ///////////////////
    /** DTD. */
    stdair::DTD_T _dtd;
  };

}
#endif // __AIRINV_BOM_FRAT5STRUCT_HPP
