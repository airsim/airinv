#ifndef __AIRINV_CMD_INVENTORYPARSERHELPER_HPP
#define __AIRINV_CMD_INVENTORYPARSERHELPER_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <string>
// StdAir
#include <stdair/command/CmdAbstract.hpp>
// Airinv
#include <airinv/AIRINV_Types.hpp>
#define BOOST_SPIRIT_DEBUG
#include <airinv/basic/BasParserTypes.hpp>
#include <airinv/bom/FlightDateStruct.hpp>

// Forward declarations
namespace stdair {
  class BomRoot;
}

namespace AIRINV {

  namespace InventoryParserHelper {
    
    // ///////////////////////////////////////////////////////////////////
    //  Semantic actions
    // ///////////////////////////////////////////////////////////////////
    /** Generic Semantic Action (Actor / Functor) for the Inventory Parser. */
    struct ParserSemanticAction {
      /** Actor Constructor. */
      ParserSemanticAction (FlightDateStruct_T&);
      /** Actor Context. */
      FlightDateStruct_T& _flightDate;
    };
      
    /** Store the snapshot date. */
    struct storeSnapshotDate : public ParserSemanticAction {
      /** Actor Constructor. */
      storeSnapshotDate (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (iterator_t iStr, iterator_t iStrEnd) const;
    };

    /** Store the parsed airline code. */
    struct storeAirlineCode : public ParserSemanticAction {
      /** Actor Constructor. */
      storeAirlineCode (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (iterator_t iStr, iterator_t iStrEnd) const;
    };
  
    /** Store the parsed flight number. */
    struct storeFlightNumber : public ParserSemanticAction {
      /** Actor Constructor. */
      storeFlightNumber (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (unsigned int iNumber) const;
    };
  
    /** Store the flight date. */
    struct storeFlightDate : public ParserSemanticAction {
      /** Actor Constructor. */
      storeFlightDate (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (iterator_t iStr, iterator_t iStrEnd) const;
    };

    /** Store the flight type code. */
    struct storeFlightTypeCode : public ParserSemanticAction {
      /** Actor Constructor. */
      storeFlightTypeCode (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (iterator_t iStr, iterator_t iStrEnd) const;
    };

    /** Store the parsed leg boarding point. */
    struct storeLegBoardingPoint : public ParserSemanticAction {
      /** Actor Constructor. */
      storeLegBoardingPoint (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (iterator_t iStr, iterator_t iStrEnd) const;
    };
  
    /** Store the parsed leg off point. */
    struct storeLegOffPoint : public ParserSemanticAction {
      /** Actor Constructor. */
      storeLegOffPoint (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (iterator_t iStr, iterator_t iStrEnd) const;
    };

    /** Store the boarding date. */
    struct storeBoardingDate : public ParserSemanticAction {
      /** Actor Constructor. */
      storeBoardingDate (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (iterator_t iStr, iterator_t iStrEnd) const;
    };

    /** Store the boarding time. */
    struct storeBoardingTime : public ParserSemanticAction {
      /** Actor Constructor. */
      storeBoardingTime (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (iterator_t iStr, iterator_t iStrEnd) const;
    };

    /** Store the off date. */
    struct storeOffDate : public ParserSemanticAction {
      /** Actor Constructor. */
      storeOffDate (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (iterator_t iStr, iterator_t iStrEnd) const;
    };

    /** Store the off time. */
    struct storeOffTime : public ParserSemanticAction {
      /** Actor Constructor. */
      storeOffTime (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (iterator_t iStr, iterator_t iStrEnd) const;
    };

    /** Store the parsed leg cabin code. */
    struct storeLegCabinCode : public ParserSemanticAction {
      /** Actor Constructor. */
      storeLegCabinCode (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (char iChar) const;
    };
  
    /** Store the parsed saleable capacity. */
    struct storeSaleableCapacity : public ParserSemanticAction {
      /** Actor Constructor. */
      storeSaleableCapacity (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (double iReal) const;
    };
  
    /** Store the parsed Authorisation Level (AU). */
    struct storeAU : public ParserSemanticAction {
      /** Actor Constructor. */
      storeAU (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (double iReal) const;
    };
  
    /** Store the parsed Unsold Protected (UPR). */
    struct storeUPR : public ParserSemanticAction {
      /** Actor Constructor. */
      storeUPR (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (double iReal) const;
    };
  
    /** Store the parsed booking counter. */
    struct storeBookingCounter : public ParserSemanticAction {
      /** Actor Constructor. */
      storeBookingCounter (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (double iReal) const;
    };
  
    /** Store the parsed Net Availability (NAV). */
    struct storeNAV : public ParserSemanticAction {
      /** Actor Constructor. */
      storeNAV (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (double iReal) const;
    };
  
    /** Store the parsed Gross Availability (GAV). */
    struct storeGAV : public ParserSemanticAction {
      /** Actor Constructor. */
      storeGAV (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (double iReal) const;
    };
  
    /** Store the parsed Average Cancellation Percentage (ACP). */
    struct storeACP : public ParserSemanticAction {
      /** Actor Constructor. */
      storeACP (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (double iReal) const;
    };
  
    /** Store the parsed Expected To Board (ETB) number. */
    struct storeETB : public ParserSemanticAction {
      /** Actor Constructor. */
      storeETB (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (double iReal) const;
    };

    /** Store the parsed Yield Upper Range value. */
    struct storeYieldUpperRange : public ParserSemanticAction {
      /** Actor Constructor. */
      storeYieldUpperRange (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (double iReal) const;
    };
    
    /** Store the parsed bucket availability. */
    struct storeBucketAvaibality : public ParserSemanticAction {
      /** Actor Constructor. */
      storeBucketAvaibality (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (double iReal) const;
    };
    
    /** Store the parsed leg-cabin seat index. */
    struct storeSeatIndex : public ParserSemanticAction {
      /** Actor Constructor. */
      storeSeatIndex (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (double iReal) const;
    };
    
    /** Store the parsed segment boarding point. */
    struct storeSegmentBoardingPoint : public ParserSemanticAction {
      /** Actor Constructor. */
      storeSegmentBoardingPoint (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (iterator_t iStr, iterator_t iStrEnd) const;
    };
  
    /** Store the parsed segment off point. */
    struct storeSegmentOffPoint : public ParserSemanticAction {
      /** Actor Constructor. */
      storeSegmentOffPoint (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (iterator_t iStr, iterator_t iStrEnd) const;
    };

    /** Store the parsed segment cabin code. */
    struct storeSegmentCabinCode : public ParserSemanticAction {
      /** Actor Constructor. */
      storeSegmentCabinCode (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (char iChar) const;
    };
    
    /** Store the parsed segment cabin number of bookings. */
    struct storeSegmentCabinBookingCounter : public ParserSemanticAction {
      /** Actor Constructor. */
      storeSegmentCabinBookingCounter (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (double iReal) const;
    };

    /** Store the parsed list of class codes. */
    struct storeClasses : public ParserSemanticAction {
      /** Actor Constructor. */
      storeClasses (FlightDateStruct_T&);
      /** Actor Function (functor). */
      void operator() (iterator_t iStr, iterator_t iStrEnd) const;
    };

    /** Mark the end of the inventory parsing. */
    struct doEndFlightDate : public ParserSemanticAction {
      /** Actor Constructor. */
      doEndFlightDate (stdair::BomRoot&, FlightDateStruct_T&);
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
       FlightDepDate;
       2010-02-08; SIN; BKK; L; 10.0; 1.0;

    Grammar:
      FlightDate ::= FlightDepDate ';' Origin ';' Destination
         EndOfFlightDate
      FlightDepDate ::= date
      EndOfFlightDate ::= ';'
     */

    /** Grammar for the inventory parser. */
    struct InventoryParser : 
      public boost::spirit::classic::grammar<InventoryParser> {

      InventoryParser (stdair::BomRoot&, FlightDateStruct_T&);

      template <typename ScannerT>
      struct definition {
        definition (InventoryParser const& self);
        
        // Instantiation of rules
        boost::spirit::classic::rule<ScannerT> flight_date_list, flight_date,
          flight_date_end, flight_key, airline_code, flight_number,
          flight_type_code, date, leg_list, leg, leg_key, leg_details,
          full_leg_cabin_details, leg_cabin_details,
          bucket_list, bucket_details,
          time, segment_list, segment, segment_key, full_segment_cabin_details,
          segment_cabin_list, segment_cabin_key, segment_cabin_details,
          class_list, class_key, class_details;

        /** Entry point of the parser. */
        boost::spirit::classic::rule<ScannerT> const& start() const;
      };

      // Parser Context
      stdair::BomRoot& _bomRoot;
      FlightDateStruct_T& _flightDate;
    };

  }


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
  class InventoryFileParser : public stdair::CmdAbstract {
  public:
    /** Constructor. */
    InventoryFileParser (stdair::BomRoot&,
                         const stdair::Filename_T& iInventoryInputFilename);

    /** Parse the inventory input file. */
    bool generateInventory ();
      
  private:
    /** Initialise. */
    void init();
      
  private:
    // Attributes
    /** File-name of the CSV-formatted inventory input file. */
    stdair::Filename_T _filename;

    /** Start iterator for the parser. */
    iterator_t _startIterator;
      
    /** End iterator for the parser. */
    iterator_t _endIterator;
      
    /** Root of the BOM tree. */
    stdair::BomRoot& _bomRoot;

    /** FlightDate Structure. */
    FlightDateStruct_T _flightDate;
  };
    
}
#endif // __AIRINV_CMD_INVENTORYPARSERHELPER_HPP
