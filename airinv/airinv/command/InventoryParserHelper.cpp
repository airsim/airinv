// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// StdAir
#include <stdair/bom/BomRoot.hpp>
#include <stdair/service/Logger.hpp>
// Airinv
#include <airinv/command/InventoryParserHelper.hpp>

namespace AIRINV {

  namespace InventoryParserHelper {
      
    // //////////////////////////////////////////////////////////////////
    //  Semantic actions
    // //////////////////////////////////////////////////////////////////

    ParserSemanticAction::
    ParserSemanticAction (InventoryStruct_T& ioInventory)
      : _inventory (ioInventory) {
    }      

    // //////////////////////////////////////////////////////////////////
    storeFlightDepartureDate::
    storeFlightDepartureDate (InventoryStruct_T& ioInventory)
      : ParserSemanticAction (ioInventory) {
    }
    
    // //////////////////////////////////////////////////////////////////
    void storeFlightDepartureDate::operator() (iterator_t iStr,
                                               iterator_t iStrEnd) const {
      _inventory._flightDate = _inventory.getFlightDate();
    }
      
    // //////////////////////////////////////////////////////////////////
    doEndInventory::doEndInventory (stdair::BomRoot& ioBomRoot,
                              InventoryStruct_T& ioInventory)
      : ParserSemanticAction (ioInventory), _bomRoot (ioBomRoot) {
    }
    
    // //////////////////////////////////////////////////////////////////
    // void doEndInventory::operator() (char iChar) const {
    void doEndInventory::operator() (iterator_t iStr,
                                     iterator_t iStrEnd) const {
        
      // DEBUG: Display the result
      // STDAIR_LOG_DEBUG ("Inventory: " << _inventory.describe());

      // Create the Inventory BOM objects
      // InventoryGenerator::createInventory (_bomRoot, _inventory);
    }

      
    // ///////////////////////////////////////////////////////////////////
    //
    //  Utility Parsers
    //
    // ///////////////////////////////////////////////////////////////////
    /** 1-digit-integer parser */
    int1_p_t int1_p;
    
    /** 2-digit-integer parser */
    uint2_p_t uint2_p;
    
    /** Up-to-2-digit-integer parser */
    uint1_2_p_t uint1_2_p;

    /** Up-to-3-digit-integer parser */
    uint1_3_p_t uint1_3_p;

    /** 4-digit-integer parser */
    uint4_p_t uint4_p;
    
    /** Up-to-4-digit-integer parser */
    uint1_4_p_t uint1_4_p;

    /** Airline Code Parser: repeat_p(2,3)[chset_p("0-9A-Z")] */
    repeat_p_t airline_code_p (chset_t("0-9A-Z").derived(), 2, 3);
      
    /** Flight Number Parser: limit_d(0u, 9999u)[uint1_4_p] */
    bounded1_4_p_t flight_number_p (uint1_4_p.derived(), 0u, 9999u);

    /** Year Parser: limit_d(2000u, 2099u)[uint4_p] */
    bounded4_p_t year_p (uint4_p.derived(), 2000u, 2099u);
      
    /** Month Parser: limit_d(1u, 12u)[uint2_p] */
    bounded2_p_t month_p (uint2_p.derived(), 1u, 12u);

    /** Day Parser: limit_d(1u, 31u)[uint2_p] */
    bounded2_p_t day_p (uint2_p.derived(), 1u, 31u);
     
    /** DOW (Day-Of-the-Week) Parser: repeat_p(7)[chset_p("0-1")] */
    repeat_p_t dow_p (chset_t("0-1").derived().derived(), 7, 7);

    /** Airport Parser: repeat_p(3)[chset_p("0-9A-Z")] */
    repeat_p_t airport_p (chset_t("0-9A-Z").derived(), 3, 3);
      
    /** Hour Parser: limit_d(0u, 23u)[uint2_p] */
    bounded1_2_p_t hours_p (uint1_2_p.derived(), 0u, 23u);

    /** Minute Parser: limit_d(0u, 59u)[uint2_p] */
    bounded2_p_t minutes_p (uint2_p.derived(), 0u, 59u);

    /** Second Parser: limit_d(0u, 59u)[uint2_p] */
    bounded2_p_t seconds_p (uint2_p.derived(), 0u, 59u);

    /** Cabin code parser: chset_p("A-Z") */
    chset_t cabin_code_p ("A-Z");

    /** Passenger type parser: chset_p("A-Z") */
    chset_t passenger_type_p ("A-Z");

    /** Family code parser */
    int1_p_t family_code_p;
      
    /** Class Code List Parser: repeat_p(1,26)[chset_p("A-Z")] */
    repeat_p_t class_code_list_p (chset_t("A-Z").derived(), 1, 26);

    /** Stay duration Parser: limit_d(0u, 999u)[uint3_p] */
    bounded1_3_p_t stay_duration_p (uint1_3_p.derived(), 0u, 999u);


    // //////////////////////////////////////////////////////////////////
    //  (Boost Spirit) Grammar Definition
    // //////////////////////////////////////////////////////////////////

    // //////////////////////////////////////////////////////////////////
    InventoryParser::InventoryParser (stdair::BomRoot& ioBomRoot,
                                InventoryStruct_T& ioInventory) 
      : _bomRoot (ioBomRoot), _inventory (ioInventory) {
    }

    // //////////////////////////////////////////////////////////////////
    template<typename ScannerT>
    InventoryParser::definition<ScannerT>::
    definition (InventoryParser const& self) {

      inventory_list = *( boost::spirit::classic::comment_p("//")
                       | boost::spirit::classic::comment_p("/*", "*/")
                       | inventory )
        ;

      inventory =
        flight_dep_date
        >> inventory_end[doEndInventory(self._bomRoot, self._inventory)]
        ;

      inventory_end =
        boost::spirit::classic::ch_p(';')
        ;
      
      flight_dep_date =
        date[storeFlightDepartureDate(self._inventory)]
        ;

      date =
        boost::spirit::classic::lexeme_d[
         (year_p)[boost::spirit::classic::assign_a(self._inventory._itYear)]
         >> '-'
         >> (month_p)[boost::spirit::classic::assign_a(self._inventory._itMonth)]
         >> '-'
         >> (day_p)[boost::spirit::classic::assign_a(self._inventory._itDay)]
         ]
        ;

      
      // BOOST_SPIRIT_DEBUG_NODE (InventoryParser);
      BOOST_SPIRIT_DEBUG_NODE (inventory_list);
      BOOST_SPIRIT_DEBUG_NODE (inventory);
      BOOST_SPIRIT_DEBUG_NODE (inventory_end);
      BOOST_SPIRIT_DEBUG_NODE (flight_dep_date);
      BOOST_SPIRIT_DEBUG_NODE (date);
    }

    // //////////////////////////////////////////////////////////////////
    template<typename ScannerT>
    boost::spirit::classic::rule<ScannerT> const&
    InventoryParser::definition<ScannerT>::start() const {
      return inventory_list;
    }
    
  }


  /////////////////////////////////////////////////////////////////////////
  //
  //  Entry class for the file parser
  //
  /////////////////////////////////////////////////////////////////////////

  // //////////////////////////////////////////////////////////////////////
  InventoryFileParser::
  InventoryFileParser (stdair::BomRoot& ioBomRoot,
                    const std::string& iFilename)
    : _filename (iFilename), _bomRoot (ioBomRoot) {
    init();
  }

  // //////////////////////////////////////////////////////////////////////
  void InventoryFileParser::init() {
    // Open the file
    _startIterator = iterator_t (_filename);

    // Check the filename exists and can be open
    if (!_startIterator) {
      STDAIR_LOG_ERROR ("The file " << _filename << " can not be open.");

      throw stdair::FileNotFoundException();
    }

    // Create an EOF iterator
    _endIterator = _startIterator.make_end();
  }
    
  // //////////////////////////////////////////////////////////////////////
  bool InventoryFileParser::generateInventory () {
    bool oResult = false;
      
    STDAIR_LOG_DEBUG ("Parsing inventory input file: " << _filename);

    // Initialise the parser (grammar) with the helper/staging structure.
    InventoryParserHelper::InventoryParser lInventoryParser (_bomRoot, _inventory);
      
    // Launch the parsing of the file and, thanks to the doEndInventory
    // call-back structure, the building of the whole BomRoot BOM
    // (i.e., including Inventory, FlightDate, LegDate, SegmentDate, etc.)
    boost::spirit::classic::parse_info<iterator_t> info =
      boost::spirit::classic::parse (_startIterator, _endIterator, lInventoryParser,
                                     boost::spirit::classic::space_p);

    // Retrieves whether or not the parsing was successful
    oResult = info.hit;
      
    const std::string hasBeenFullyReadStr = (info.full == true)?"":"not ";
    if (oResult == true) {
      STDAIR_LOG_DEBUG ("Parsing of inventory input file: " << _filename
                       << " succeeded: read " << info.length
                       << " characters. The input file has "
                       << hasBeenFullyReadStr
                       << "been fully read. Stop point: " << info.stop);
        
    } else {
      STDAIR_LOG_ERROR ("Parsing of inventory input file: " << _filename
                       << " failed: read " << info.length
                       << " characters. The input file has "
                       << hasBeenFullyReadStr
                       << "been fully read. Stop point: " << info.stop);
      throw ParserException();
    }

    return oResult;
  }
    
}
