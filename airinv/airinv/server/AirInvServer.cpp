// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// Boost
#include <boost/thread.hpp>
#include <boost/bind.hpp>
// AirInv
#include <airinv/server/AirInvServer.hpp>

namespace AIRINV {

  // Type definitions
  typedef boost::shared_ptr<boost::thread> ThreadShrPtr_T;
  typedef std::vector<ThreadShrPtr_T> ThreadShrPtrList_T;
  
  
  // //////////////////////////////////////////////////////////////////////
  AirInvServer::AirInvServer (const std::string& address,
                              const std::string& port,
                              const stdair::AirlineCode_T& iAirlineCode,
                              std::size_t iThreadPoolSize)
    : _threadPoolSize (iThreadPoolSize), _acceptor (_ioService),
      _newConnection (new Connection (_ioService, _requestHandler)),
      _requestHandler (iAirlineCode) {
    
    // Open the acceptor with the option to reuse the address
    // (i.e. SO_REUSEADDR).
    boost::asio::ip::tcp::resolver resolver (_ioService);
    boost::asio::ip::tcp::resolver::query query (address, port);
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

    _acceptor.open (endpoint.protocol());
    _acceptor.set_option (boost::asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor.bind (endpoint);
    _acceptor.listen();
    
    assert (_newConnection != NULL);
    _acceptor.async_accept (_newConnection->socket(),
                            boost::bind (&AirInvServer::handleAccept, this,
                                         boost::asio::placeholders::error));
  }

  // //////////////////////////////////////////////////////////////////////
  AirInvServer::~AirInvServer () {
  }
  
  // //////////////////////////////////////////////////////////////////////
  void AirInvServer::run() {
    // Create a pool of threads to run all of the io_services.
    ThreadShrPtrList_T lThreadList;
  
    for (std::size_t itThread = 0; itThread != _threadPoolSize; ++itThread) {
      ThreadShrPtr_T lThread (new boost::thread (boost::bind (&boost::asio::io_service::run,
                                                              &_ioService)));
      lThreadList.push_back (lThread);
    }

    // Wait for all threads in the pool to exit.
    for (std::size_t itThread = 0; itThread != lThreadList.size(); ++itThread) {
      boost::shared_ptr<boost::thread> lThread_ptr = lThreadList.at (itThread);
      assert (lThread_ptr != NULL);
      lThread_ptr->join();
    }
  }

  // //////////////////////////////////////////////////////////////////////
  void AirInvServer::stop() {
    _ioService.stop();
  }

  // //////////////////////////////////////////////////////////////////////
  void AirInvServer::handleAccept (const boost::system::error_code& iError) {
    
    if (!iError) {

      assert (_newConnection != NULL);
      
      _newConnection->start();

      _newConnection.reset (new Connection (_ioService, _requestHandler));
      
      _acceptor.async_accept (_newConnection->socket(),
                              boost::bind (&AirInvServer::handleAccept, this,
                                           boost::asio::placeholders::error));
    }
  }

}
