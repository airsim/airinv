// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <ostream>
// StdAir
#include <stdair/stdair_exceptions.hpp>
#include <stdair/stdair_types.hpp>
#include <stdair/bom/BomRoot.hpp>
#include <stdair/service/Logger.hpp>
// #define BOOST_SPIRIT_DEBUG
#include <airinv/command/FRAT5ParserHelper.hpp>

//
namespace bsc = boost::spirit::classic;

namespace AIRINV {

  namespace FRAT5ParserHelper {
      
    // //////////////////////////////////////////////////////////////////
    //  Semantic actions
    // //////////////////////////////////////////////////////////////////

    ParserSemanticAction::
    ParserSemanticAction (FRAT5Struct& ioFRAT5)
      : _frat5 (ioFRAT5) {
    }      

    // //////////////////////////////////////////////////////////////////
    storeCurveKey::
    storeCurveKey (FRAT5Struct& ioFRAT5)
      : ParserSemanticAction (ioFRAT5) {
    }
    
    // //////////////////////////////////////////////////////////////////
    void storeCurveKey::operator() (iterator_t iStr,
                                    iterator_t iStrEnd) const { 
      const std::string lKey (iStr, iStrEnd);
      _frat5._key = lKey;
      //STDAIR_LOG_DEBUG ("Key: " << lKey);
    }

    // //////////////////////////////////////////////////////////////////
    storeDTD::storeDTD (FRAT5Struct& ioFRAT5)
      : ParserSemanticAction (ioFRAT5) {
    }
    
    // //////////////////////////////////////////////////////////////////
    void storeDTD::operator() (int iDTD) const {
      _frat5._dtd = iDTD;
      //STDAIR_LOG_DEBUG ("DTD: " << iDTD);
    }

    // //////////////////////////////////////////////////////////////////
    storeFRAT5Value::storeFRAT5Value (FRAT5Struct& ioFRAT5)
      : ParserSemanticAction (ioFRAT5) {
    }
    
    // //////////////////////////////////////////////////////////////////
    void storeFRAT5Value::operator() (double iReal) const {
      const bool hasInsertBeenSuccessfull = 
        _frat5._curve.
        insert (stdair::FRAT5Curve_T::
                value_type (_frat5._dtd, iReal)).second;
      if (hasInsertBeenSuccessfull == false) {
        std::ostringstream oStr;
        oStr << "The same DTD ('" << _frat5._dtd
             << "') has probably been given twice";
        STDAIR_LOG_ERROR (oStr.str());
        throw stdair::KeyDuplicationException (oStr.str());
      }
      
      //STDAIR_LOG_DEBUG ("Value: " << iReal);
    }    

    // //////////////////////////////////////////////////////////////////
    doEndCurve::
    doEndCurve (stdair::BomRoot& ioBomRoot,
                 FRAT5Struct& ioFRAT5)
      : ParserSemanticAction (ioFRAT5),
        _bomRoot (ioBomRoot) {
    }
    
    // //////////////////////////////////////////////////////////////////
    // void doEndCurve::operator() (char iChar) const {
    void doEndCurve::operator() (iterator_t iStr,
                                 iterator_t iStrEnd) const {
      // DEBUG: Display the result
      STDAIR_LOG_DEBUG ("FRAT5: " << _frat5.describe());

      // Add the curve to the BomRoot.
      _bomRoot.addFRAT5Curve (_frat5._key, _frat5._curve);
                
      // As that's the end of a curve, the values must be cleared.
      _frat5._curve.clear();
    }

      
    // ///////////////////////////////////////////////////////////////////
    //
    //  Utility Parsers
    //
    // ///////////////////////////////////////////////////////////////////
    /** Key Parser: repeat_p(1,10)[chset_p("0-9A-Z")] */
    repeat_p_t key_p (chset_t("0-9A-Z").derived(), 1, 10);

    // //////////////////////////////////////////////////////////////////
    //  (Boost Spirit) Grammar Definition
    // //////////////////////////////////////////////////////////////////

    // //////////////////////////////////////////////////////////////////
    FRAT5Parser::
    FRAT5Parser (stdair::BomRoot& ioBomRoot,
                        FRAT5Struct& ioFRAT5) 
      : _bomRoot (ioBomRoot),
        _frat5 (ioFRAT5) {
    }

    // //////////////////////////////////////////////////////////////////
    template<typename ScannerT>
    FRAT5Parser::definition<ScannerT>::
    definition (FRAT5Parser const& self) {

      curve_list = *( not_to_be_parsed | curve )
        ;
      
      not_to_be_parsed =
        bsc::lexeme_d[ bsc::comment_p("//") | bsc::comment_p("/*", "*/")
                       | bsc::space_p ]
        ;

      curve = key >> ';' >> map
                  >> curve_end[doEndCurve(self._bomRoot, self._frat5)]
        ;

      curve_end = bsc::ch_p(';')
        ;
      
      key =
        bsc::lexeme_d[(key_p)[storeCurveKey(self._frat5)]]
        ;

      map =
        value_pair >> *( ';' >> value_pair)
        ;

      value_pair = bsc::uint_p[storeDTD(self._frat5)]
        >> ":" >> bsc::ureal_p[storeFRAT5Value(self._frat5)]
        ;
        
      // BOOST_SPIRIT_DEBUG_NODE (FRAT5Parser);
      BOOST_SPIRIT_DEBUG_NODE (curve_list);
      BOOST_SPIRIT_DEBUG_NODE (not_to_be_parsed);
      BOOST_SPIRIT_DEBUG_NODE (key);
      BOOST_SPIRIT_DEBUG_NODE (map);
      BOOST_SPIRIT_DEBUG_NODE (value_pair);
    }

    // //////////////////////////////////////////////////////////////////
    template<typename ScannerT>
    bsc::rule<ScannerT> const&
    FRAT5Parser::definition<ScannerT>::start() const {
      return curve_list;
    }    
  }


  /////////////////////////////////////////////////////////////////////////
  //
  //  Entry class for the file parser
  //
  /////////////////////////////////////////////////////////////////////////

  // //////////////////////////////////////////////////////////////////////
  FRAT5FileParser::
  FRAT5FileParser (stdair::BomRoot& ioBomRoot,
                          const stdair::Filename_T& iFilename)
    : _filename (iFilename), _bomRoot (ioBomRoot) {
    init();
  }

  // //////////////////////////////////////////////////////////////////////
  void FRAT5FileParser::init() {
    // Open the file
    _startIterator = iterator_t (_filename);

    // Check the filename exists and can be open
    if (!_startIterator) {
      std::ostringstream oMessage;
      oMessage << "The file " << _filename << " can not be open." << std::endl;
      STDAIR_LOG_ERROR (oMessage.str());
      throw FRAT5InputFileNotFoundException (oMessage.str());
    }

    // Create an EOF iterator
    _endIterator = _startIterator.make_end();
  }
    
  // //////////////////////////////////////////////////////////////////////
  bool FRAT5FileParser::generateFRAT5Curves () {
    bool oResult = false;
      
    STDAIR_LOG_DEBUG ("Parsing FRAT5 input file: " << _filename);

    // Initialise the parser (grammar) with the helper/staging structure.
    FRAT5ParserHelper::FRAT5Parser lFRAT5Parser (_bomRoot, _frat5);
      
    // Launch the parsing of the file and, thanks to the doEndCurve
    // call-back structure, the building of the whole BomRoot BOM
    // (i.e., including Inventory, FlightDate, LegDate, SegmentDate, etc.)
    bsc::parse_info<iterator_t> info = bsc::parse (_startIterator, _endIterator,
                                                   lFRAT5Parser,
                                                   bsc::space_p - bsc::eol_p);

    // Retrieves whether or not the parsing was successful
    oResult = info.hit;

    const bool isFull = info.full;
      
    const std::string hasBeenFullyReadStr = (isFull  == true)?"":"not ";
    if (oResult == true && isFull == true) {
      STDAIR_LOG_DEBUG ("Parsing of FRAT5 input file: " << _filename
                       << " succeeded: read " << info.length
                       << " characters. The input file has "
                       << hasBeenFullyReadStr
                       << "been fully read. Stop point: " << info.stop);
        
    } else {
      STDAIR_LOG_ERROR ("Parsing of FRAT5 input file: " << _filename
                       << " failed: read " << info.length
                       << " characters. The input file has "
                       << hasBeenFullyReadStr
                       << "been fully read. Stop point: " << info.stop);
      throw FRAT5FileParsingFailedException ("Parsing of FRAT5 input file: "
                                             + _filename + " failed.");
    }

    return oResult;
  }
    
}
