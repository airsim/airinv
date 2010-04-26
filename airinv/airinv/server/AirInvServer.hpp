#ifndef __AIRINV_SVR_AIRINVSERVER_HPP
#define __AIRINV_SVR_AIRINVSERVER_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <string>
#include <vector>
// Boost
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
// StdAir
#include <stdair/STDAIR_Types.hpp>
#include <stdair/bom/Inventory.hpp>
// AirInv
#include <airinv/server/connection.hpp>
#include <airinv/server/request_handler.hpp>

namespace AIRINV {

  /** The top-level class of the AirInv server. */
  class AirInvServer : private boost::noncopyable {
  public:
    /** Constructor.
        <br>Construct the server to listen on the specified TCP address
        and port, and serve up files from the given directory. */
    AirInvServer (const std::string& address, const std::string& port,
                  const stdair::AirlineCode_T& iAirlineCode,
                  std::size_t thread_pool_size);
    /** Destructor. */
    ~AirInvServer();


  public:
    // ////////////////// Business Methods ////////////////////
    /** Run the server's io_service loop. */
    void run();

    /** Stop the server. */
    void stop();

    
  private:
    // //////////// Constructors and Destructors /////////////////
    /** Default constructors. */
    AirInvServer();
    AirInvServer(const AirInvServer&);

    
  private:
    // //////////// Attributes /////////////////
    /// Handle completion of an asynchronous accept operation.
    void handle_accept (const boost::system::error_code& e);

    /// The number of threads that will call io_service::run().
    std::size_t thread_pool_size_;

    /// The io_service used to perform asynchronous operations.
    boost::asio::io_service io_service_;

    /// Acceptor used to listen for incoming connections.
    boost::asio::ip::tcp::acceptor acceptor_;

    /// The next connection to be accepted.
    connection_ptr new_connection_;

    /// The handler for all incoming requests.
    request_handler request_handler_;
  };

}
#endif // __AIRINV_SVR_AIRINVSERVER_HPP
