#ifndef __AIRINV_CMD_FRAT5PARSERHELPER_HPP
#define __AIRINV_CMD_FRAT5PARSERHELPER_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <string>
// StdAir
#include <stdair/command/CmdAbstract.hpp>
// Airinv
#include <airinv/AIRINV_Types.hpp>
#include <airinv/basic/BasParserTypes.hpp>
#include <airinv/bom/FRAT5Struct.hpp>

// Forward declarations
namespace stdair {
  class BomRoot;
}

namespace AIRINV {

  namespace FRAT5ParserHelper {
    
    // ///////////////////////////////////////////////////////////////////
    //  Semantic actions
    // ///////////////////////////////////////////////////////////////////
    /** Generic Semantic Action (Actor / Functor) for the FRAT5 Parser. */
    struct ParserSemanticAction {
      /** Actor Constructor. */
      ParserSemanticAction (FRAT5Struct&);
      /** Actor Context. */
      FRAT5Struct& _frat5;
    };
      
    /** Store the parsed curve key. */
    struct storeCurveKey : public ParserSemanticAction {
      /** Actor Constructor. */
      storeCurveKey (FRAT5Struct&);
      /** Actor Function (functor). */
      void operator() (iterator_t iStr, iterator_t iStrEnd) const;
    };
  
    /** Store the DTD. */
    struct storeDTD : public ParserSemanticAction {
      /** Actor Constructor. */
      storeDTD (FRAT5Struct&);
      /** Actor Function (functor). */
      void operator() (int iDTD) const;
    };
  
    /** Store the FRAT5 value. */
    struct storeFRAT5Value : public ParserSemanticAction {
      /** Actor Constructor. */
      storeFRAT5Value (FRAT5Struct&);
      /** Actor Function (functor). */
      void operator() (double iReal) const;
    };
      
    /** Mark the end of the FRAT5 curve parsing. */
    struct doEndCurve : public ParserSemanticAction {
      /** Actor Constructor. */
      doEndCurve (stdair::BomRoot&, FRAT5Struct&);
      /** Actor Function (functor). */
      void operator() (iterator_t iStr, iterator_t iStrEnd) const;
      /** Actor Specific Context. */
      stdair::BomRoot& _bomRoot;
    };

    /////////////////////////////////////////////////////////////////////////
    //
    //  (Boost Spirit) Grammar Definition
    //
    /////////////////////////////////////////////////////////////////////////
    /**
       CurveKey; FRAT5Curve;
       F1; 63:1.40; 56:1.45; 49:1.50; 42:1.55; 35:1.60; 31:1.70; 27:1.80; 23:2.00; 19:2.30; 16:2.60; 13:3.00; 10:3.30; 7:3.40; 5:3.44; 3:3.47; 1:3.50;
       
       Grammar:
       Curve               ::= Key ';' FRAT5Map
                           EndOfCurve
       Key                 ::= string
       FRAT5Map            ::= DTDValuePaire (';' DTDValuePaire)*
       DTDValuePaire       ::= DTD ':' FRAT5Value
       EndOfCurve          ::= ';'
    */

    /** Grammar for the FRAT5 curve parser. */
    struct FRAT5Parser : 
      public boost::spirit::classic::grammar<FRAT5Parser> {

      FRAT5Parser (stdair::BomRoot&, FRAT5Struct&);

      template <typename ScannerT>
      struct definition {
        definition (FRAT5Parser const& self);

        // Instantiation of rules
        boost::spirit::classic::rule<ScannerT> curve_list,
          not_to_be_parsed, curve, key, map, value_pair, curve_end;

        /** Entry point of the parser. */
        boost::spirit::classic::rule<ScannerT> const& start() const;
      };

      // Parser Context
      stdair::BomRoot& _bomRoot;
      FRAT5Struct& _frat5;
    };
  }

  /** Short Description
      <br> Detailed Description.*/
  

  /////////////////////////////////////////////////////////////////////////
  //
  //  Entry class for the file parser
  //
  /////////////////////////////////////////////////////////////////////////
  /** Class wrapping the initialisation and entry point of the parser.
      <br>The seemingly redundancy is used to force the instantiation of
      the actual parser, which is a templatised Boost Spirit grammar.
      Hence, the actual parser is instantiated within that class object
      code. */
  class FRAT5FileParser : public stdair::CmdAbstract {
  public:
    /** Constructor. */
    FRAT5FileParser (stdair::BomRoot& ioBomRoot,
                     const stdair::Filename_T& iFilename);

    /** Parse the input file and generate the FRAT5 curves. */
    bool generateFRAT5Curves ();
      
  private:
    /** Initialise. */
    void init();
      
  private:
    // Attributes
    /** File-name of the CSV-formatted FRAT5 input file. */
    stdair::Filename_T _filename;

    /** Start iterator for the parser. */
    iterator_t _startIterator;
      
    /** End iterator for the parser. */
    iterator_t _endIterator;
      
    /** stdair::BomRoot. */
    stdair::BomRoot& _bomRoot;

    /** FRAT5 Structure. */
    FRAT5Struct _frat5;
  };
    
}
#endif // __AIRINV_CMD_FRAT5PARSERHELPER_HPP
