#ifndef __AIRINV_SVR_REQUEST_HANDLER_HPP
#define __AIRINV_SVR_REQUEST_HANDLER_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <string>
// Boost
#include <boost/noncopyable.hpp>
// StdAir
#include <stdair/STDAIR_Types.hpp>
// AirInv

namespace AIRINV {

  // Forward declarations.
  struct reply;
  struct request;

  /// The common handler for all incoming requests.
  class request_handler
    : private boost::noncopyable {
  public:
    /// Construct with a directory containing files to be served.
    request_handler (const stdair::AirlineCode_T&);

    /// Handle a request and produce a reply.
    void handle_request (const request& req, reply& rep);

  private:
    /// The directory containing the files to be served.
    stdair::AirlineCode_T doc_root_;

    /// Perform URL-decoding on a string. Returns false if the encoding was
    /// invalid.
    static bool url_decode (const std::string& in, std::string& out);
  };

}
#endif // __AIRINV_SVR_REQUEST_HANDLER_HPP
