#ifndef __AIRINV_AIRINV_TYPES_HPP
#define __AIRINV_AIRINV_TYPES_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <exception>
#include <string>
#include <map>
// Boost
#include <boost/shared_ptr.hpp>

namespace AIRINV {

  // Forward declarations
  class AIRINV_Service;

  // ///////// Exceptions ///////////
  class RootException : public std::exception {
  };

  class NonInitialisedServiceException : public RootException {
  };

  class MemoryAllocationException : public RootException {
  };

  class ObjectNotFoundException : public RootException {
  };

  class ParserException : public RootException {
  };

  class CodeConversionException : public ParserException {
  };

  class CodeDuplicationException : public ParserException {
  };

  class SQLDatabaseException : public RootException {
  };

  class SQLDatabaseConnectionImpossibleException : public SQLDatabaseException {
  };

  class BookingException : public RootException {
  };


  // /////////////// Log /////////////
  /** Level of logs. */
  namespace LOG {
    typedef enum {
      CRITICAL = 0,
      ERROR,
      NOTIFICATION,
      WARNING,
      DEBUG,
      VERBOSE,
      LAST_VALUE
    } EN_LogLevel;
  }


  // //////// Type definitions /////////
  /** Pointer on the AIRINV Service handler. */
  typedef boost::shared_ptr<AIRINV::AIRINV_Service> AIRINV_ServicePtr_T;
  
  /** Typedef which defines a map of airline codes and the corresponding
      airline inventories. */
  typedef std::map<const stdair::AirlineCode_T,
                   AIRINV_ServicePtr_T> AIRINV_ServicePtr_Map_T;
}
#endif // __AIRINV_AIRINV_TYPES_HPP

