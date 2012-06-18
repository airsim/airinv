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
#include <airinv/command/FFDisutilityParserHelper.hpp>

//
namespace bsc = boost::spirit::classic;

namespace AIRINV {

  namespace FFDisutilityParserHelper {
      
    // //////////////////////////////////////////////////////////////////
    //  Semantic actions
    // //////////////////////////////////////////////////////////////////

    ParserSemanticAction::
    ParserSemanticAction (FFDisutilityStruct& ioFFDisutility)
      : _ffDisutility (ioFFDisutility) {
    }      

    // //////////////////////////////////////////////////////////////////
    storeCurveKey::
    storeCurveKey (FFDisutilityStruct& ioFFDisutility)
      : ParserSemanticAction (ioFFDisutility) {
    }
    
    // //////////////////////////////////////////////////////////////////
    void storeCurveKey::operator() (iterator_t iStr,
                                    iterator_t iStrEnd) const { 
      const std::string lKey (iStr, iStrEnd);
      _ffDisutility._key = lKey;
    }

    // //////////////////////////////////////////////////////////////////
    storeDTD::storeDTD (FFDisutilityStruct& ioFFDisutility)
      : ParserSemanticAction (ioFFDisutility) {
    }
    
    // //////////////////////////////////////////////////////////////////
    void storeDTD::operator() (int iDTD) const {
      _ffDisutility._dtd = iDTD;
      //STDAIR_LOG_DEBUG ("DTD: " << iDTD);
    }

    // //////////////////////////////////////////////////////////////////
    storeFFDisutilityValue::storeFFDisutilityValue (FFDisutilityStruct& ioFFDisutility)
      : ParserSemanticAction (ioFFDisutility) {
    }
    
    // //////////////////////////////////////////////////////////////////
    void storeFFDisutilityValue::operator() (double iReal) const {
      const bool hasInsertBeenSuccessfull = 
        _ffDisutility._curve.
        insert (stdair::FFDisutilityCurve_T::
                value_type (_ffDisutility._dtd, iReal)).second;
      if (hasInsertBeenSuccessfull == false) {
        std::ostringstream oStr;
        oStr << "The same DTD ('" << _ffDisutility._dtd
             << "') has probably been given twice";
        STDAIR_LOG_ERROR (oStr.str());
        throw stdair::KeyDuplicationException (oStr.str());
      }
      
      //STDAIR_LOG_DEBUG ("PosProbMass: " << iReal);
    }    

    // //////////////////////////////////////////////////////////////////
    doEndCurve::
    doEndCurve (stdair::BomRoot& ioBomRoot,
                 FFDisutilityStruct& ioFFDisutility)
      : ParserSemanticAction (ioFFDisutility),
        _bomRoot (ioBomRoot) {
    }
    
    // //////////////////////////////////////////////////////////////////
    // void doEndCurve::operator() (char iChar) const {
    void doEndCurve::operator() (iterator_t iStr,
                                 iterator_t iStrEnd) const {
      // DEBUG: Display the result
      STDAIR_LOG_DEBUG ("FFDisutility: " << _ffDisutility.describe());

      // Add the curve to the BomRoot.
      _bomRoot.addFFDisutilityCurve (_ffDisutility._key, _ffDisutility._curve);
                
      // As that's the end of a curve, the values must be cleared.
      _ffDisutility._curve.clear();
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
    FFDisutilityParser::
    FFDisutilityParser (stdair::BomRoot& ioBomRoot,
                        FFDisutilityStruct& ioFFDisutility) 
      : _bomRoot (ioBomRoot),
        _ffDisutility (ioFFDisutility) {
    }

    // //////////////////////////////////////////////////////////////////
    template<typename ScannerT>
    FFDisutilityParser::definition<ScannerT>::
    definition (FFDisutilityParser const& self) {

      curve_list = *( not_to_be_parsed | curve )
        ;
      
      not_to_be_parsed =
        bsc::lexeme_d[ bsc::comment_p("//") | bsc::comment_p("/*", "*/")
                       | bsc::space_p ]
        ;

      curve = key >> ';' >> map
                  >> curve_end[doEndCurve(self._bomRoot, self._ffDisutility)]
        ;

      curve_end = bsc::ch_p(';')
        ;
      
      key =
        bsc::lexeme_d[(key_p)[storeCurveKey(self._ffDisutility)]]
        ;

      map =
        value_pair >> *( ';' >> value_pair)
        ;

      value_pair = bsc::uint_p[storeDTD(self._ffDisutility)]
        >> ":" >> bsc::ureal_p[storeFFDisutilityValue(self._ffDisutility)]
        ;
        
      // BOOST_SPIRIT_DEBUG_NODE (FFDisutilityParser);
      BOOST_SPIRIT_DEBUG_NODE (curve_list);
      BOOST_SPIRIT_DEBUG_NODE (not_to_be_parsed);
      BOOST_SPIRIT_DEBUG_NODE (key);
      BOOST_SPIRIT_DEBUG_NODE (map);
      BOOST_SPIRIT_DEBUG_NODE (value_pair);
    }

    // //////////////////////////////////////////////////////////////////
    template<typename ScannerT>
    bsc::rule<ScannerT> const&
    FFDisutilityParser::definition<ScannerT>::start() const {
      return curve_list;
    }    
  }


  /////////////////////////////////////////////////////////////////////////
  //
  //  Entry class for the file parser
  //
  /////////////////////////////////////////////////////////////////////////

  // //////////////////////////////////////////////////////////////////////
  FFDisutilityFileParser::
  FFDisutilityFileParser (stdair::BomRoot& ioBomRoot,
                          const stdair::Filename_T& iFilename)
    : _filename (iFilename), _bomRoot (ioBomRoot) {
    init();
  }

  // //////////////////////////////////////////////////////////////////////
  void FFDisutilityFileParser::init() {
    // Open the file
    _startIterator = iterator_t (_filename);

    // Check the filename exists and can be open
    if (!_startIterator) {
      std::ostringstream oMessage;
      oMessage << "The file " << _filename << " can not be open." << std::endl;
      STDAIR_LOG_ERROR (oMessage.str());
      throw FFDisutilityInputFileNotFoundException (oMessage.str());
    }

    // Create an EOF iterator
    _endIterator = _startIterator.make_end();
  }
    
  // //////////////////////////////////////////////////////////////////////
  bool FFDisutilityFileParser::generateFFDisutilityCurves () {
    bool oResult = false;
      
    STDAIR_LOG_DEBUG ("Parsing FFDisutility input file: " << _filename);

    // Initialise the parser (grammar) with the helper/staging structure.
    FFDisutilityParserHelper::FFDisutilityParser lFFDisutilityParser (_bomRoot, _ffDisutility);
      
    // Launch the parsing of the file and, thanks to the doEndCurve
    // call-back structure, the building of the whole BomRoot BOM
    // (i.e., including Inventory, FlightDate, LegDate, SegmentDate, etc.)
    bsc::parse_info<iterator_t> info = bsc::parse (_startIterator, _endIterator,
                                                   lFFDisutilityParser,
                                                   bsc::space_p - bsc::eol_p);

    // Retrieves whether or not the parsing was successful
    oResult = info.hit;

    const bool isFull = info.full;
      
    const std::string hasBeenFullyReadStr = (isFull  == true)?"":"not ";
    if (oResult == true && isFull == true) {
      STDAIR_LOG_DEBUG ("Parsing of FFDisutility input file: " << _filename
                       << " succeeded: read " << info.length
                       << " characters. The input file has "
                       << hasBeenFullyReadStr
                       << "been fully read. Stop point: " << info.stop);
        
    } else {
      STDAIR_LOG_ERROR ("Parsing of FFDisutility input file: " << _filename
                       << " failed: read " << info.length
                       << " characters. The input file has "
                       << hasBeenFullyReadStr
                       << "been fully read. Stop point: " << info.stop);
      throw FFDisutilityFileParsingFailedException ("Parsing of FFDisutility input file: " + _filename + " failed.");
    }

    return oResult;
  }
    
}
