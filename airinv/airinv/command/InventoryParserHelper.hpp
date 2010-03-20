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
#include <airinv/basic/BasParserTypes.hpp>
#include <airinv/bom/InventoryStruct.hpp>

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
      ParserSemanticAction (InventoryStruct_T&);
      /** Actor Context. */
      InventoryStruct_T& _inventory;
    };
      
    /** Store the flight departure date. */
    struct storeFlightDepartureDate : public ParserSemanticAction {
      /** Actor Constructor. */
      storeFlightDepartureDate (InventoryStruct_T&);
      /** Actor Function (functor). */
      void operator() (iterator_t iStr, iterator_t iStrEnd) const;
    };

    /** Mark the end of the inventory parsing. */
    struct doEndInventory : public ParserSemanticAction {
      /** Actor Constructor. */
      doEndInventory (stdair::BomRoot&, InventoryStruct_T&);
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
      Inventory ::= FlightDepDate ';' Origin ';' Destination
         EndOfInventory
      FlightDepDate ::= date
      EndOfInventory ::= ';'
     */

    /** Grammar for the inventory parser. */
    struct InventoryParser : 
      public boost::spirit::classic::grammar<InventoryParser> {

      InventoryParser (stdair::BomRoot&, InventoryStruct_T&);

      template <typename ScannerT>
      struct definition {
        definition (InventoryParser const& self);
        
        // Instantiation of rules
        boost::spirit::classic::rule<ScannerT> inventory_list, inventory,
          inventory_end, flight_dep_date, date;

        /** Entry point of the parser. */
        boost::spirit::classic::rule<ScannerT> const& start() const;
      };

      // Parser Context
      stdair::BomRoot& _bomRoot;
      InventoryStruct_T& _inventory;
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

    /** Inventory Structure. */
    InventoryStruct_T _inventory;
  };
    
}
#endif // __AIRINV_CMD_INVENTORYPARSERHELPER_HPP
