#ifndef __AIRINV_CMD_FFDISUTILITYPARSERHELPER_HPP
#define __AIRINV_CMD_FFDISUTILITYPARSERHELPER_HPP

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
#include <airinv/bom/FFDisutilityStruct.hpp>

// Forward declarations
namespace stdair {
  class BomRoot;
}

namespace AIRINV {

  namespace FFDisutilityParserHelper {
    
    // ///////////////////////////////////////////////////////////////////
    //  Semantic actions
    // ///////////////////////////////////////////////////////////////////
    /** Generic Semantic Action (Actor / Functor) for the FFDisutility Parser. */
    struct ParserSemanticAction {
      /** Actor Constructor. */
      ParserSemanticAction (FFDisutilityStruct&);
      /** Actor Context. */
      FFDisutilityStruct& _ffDisutility;
    };
      
    /** Store the parsed curve key. */
    struct storeCurveKey : public ParserSemanticAction {
      /** Actor Constructor. */
      storeCurveKey (FFDisutilityStruct&);
      /** Actor Function (functor). */
      void operator() (iterator_t iStr, iterator_t iStrEnd) const;
    };
  
    /** Store the DTD. */
    struct storeDTD : public ParserSemanticAction {
      /** Actor Constructor. */
      storeDTD (FFDisutilityStruct&);
      /** Actor Function (functor). */
      void operator() (int iDTD) const;
    };
  
    /** Store the FFDisutility value. */
    struct storeFFDisutilityValue : public ParserSemanticAction {
      /** Actor Constructor. */
      storeFFDisutilityValue (FFDisutilityStruct&);
      /** Actor Function (functor). */
      void operator() (double iReal) const;
    };
      
    /** Mark the end of the FFDisutility curve parsing. */
    struct doEndCurve : public ParserSemanticAction {
      /** Actor Constructor. */
      doEndCurve (stdair::BomRoot&, FFDisutilityStruct&);
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
       CurveKey; FFDisutilityCurve;
       D1; 63:0.0050; 56:0.0049; 49:0.0047; 42:0.0045; 35:0.0043; 31:0.0040; 27:0.0037; 23:0.0034; 19:0.0030; 16:0.0026; 13:0.0022; 10:0.0017; 7:0.0013; 5:0.0012; 3:0.0011; 1:0.0010;
       
       Grammar:
       Curve               ::= Key ';' FFDisutilityMap
                           EndOfCurve
       Key                 ::= string
       FFDisutilityMap            ::= DTDValuePaire (';' DTDValuePaire)*
       DTDValuePaire       ::= DTD ':' FFDisutilityValue
       EndOfCurve          ::= ';'
    */

    /** Grammar for the FFDisutility curve parser. */
    struct FFDisutilityParser : 
      public boost::spirit::classic::grammar<FFDisutilityParser> {

      FFDisutilityParser (stdair::BomRoot&, FFDisutilityStruct&);

      template <typename ScannerT>
      struct definition {
        definition (FFDisutilityParser const& self);

        // Instantiation of rules
        boost::spirit::classic::rule<ScannerT> curve_list,
          not_to_be_parsed, curve, key, map, value_pair, curve_end;

        /** Entry point of the parser. */
        boost::spirit::classic::rule<ScannerT> const& start() const;
      };

      // Parser Context
      stdair::BomRoot& _bomRoot;
      FFDisutilityStruct& _ffDisutility;
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
  class FFDisutilityFileParser : public stdair::CmdAbstract {
  public:
    /** Constructor. */
    FFDisutilityFileParser (stdair::BomRoot& ioBomRoot,
                     const stdair::Filename_T& iFilename);

    /** Parse the input file and generate the FFDisutility curves. */
    bool generateFFDisutilityCurves ();
      
  private:
    /** Initialise. */
    void init();
      
  private:
    // Attributes
    /** File-name of the CSV-formatted FFDisutility input file. */
    stdair::Filename_T _filename;

    /** Start iterator for the parser. */
    iterator_t _startIterator;
      
    /** End iterator for the parser. */
    iterator_t _endIterator;
      
    /** stdair::BomRoot. */
    stdair::BomRoot& _bomRoot;

    /** FFDisutility Structure. */
    FFDisutilityStruct _ffDisutility;
  };
    
}
#endif // __AIRINV_CMD_FFDISUTILITYPARSERHELPER_HPP
