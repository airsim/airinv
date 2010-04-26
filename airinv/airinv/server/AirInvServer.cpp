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

  // //////////////////////////////////////////////////////////////////////
  AirInvServer::AirInvServer (const std::string& address,
                              const std::string& port,
                              const stdair::AirlineCode_T& iAirlineCode,
                              std::size_t thread_pool_size)
    : thread_pool_size_(thread_pool_size),
      acceptor_(io_service_),
      new_connection_ (new connection (io_service_, request_handler_)),
      request_handler_ (iAirlineCode) {
    
    // Open the acceptor with the option to reuse the address
    // (i.e. SO_REUSEADDR).
    boost::asio::ip::tcp::resolver resolver(io_service_);
    boost::asio::ip::tcp::resolver::query query(address, port);
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

    acceptor_.open (endpoint.protocol());
    acceptor_.set_option (boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind (endpoint);
    acceptor_.listen();
    acceptor_.async_accept (new_connection_->socket(),
                            boost::bind (&AirInvServer::handle_accept, this,
                                         boost::asio::placeholders::error));
  }

  // //////////////////////////////////////////////////////////////////////
  AirInvServer::~AirInvServer () {
  }
  
  // //////////////////////////////////////////////////////////////////////
  void AirInvServer::run() {
    // Create a pool of threads to run all of the io_services.
    std::vector<boost::shared_ptr<boost::thread> > threads;
  
    for (std::size_t i = 0; i < thread_pool_size_; ++i) {
      boost::shared_ptr<boost::thread> thread (new boost::thread(
                                                                 boost::bind(&boost::asio::io_service::run, &io_service_)));
      threads.push_back(thread);
    }

    // Wait for all threads in the pool to exit.
    for (std::size_t itThread = 0; itThread != threads.size(); ++itThread) {
      boost::shared_ptr<boost::thread> lThread_ptr = threads.at (itThread);
      assert (lThread_ptr != NULL);
      lThread_ptr->join();
    }
  }

  // //////////////////////////////////////////////////////////////////////
  void AirInvServer::stop() {
    io_service_.stop();
  }

  // //////////////////////////////////////////////////////////////////////
  void AirInvServer::handle_accept(const boost::system::error_code& e) {
    if (!e) {
      new_connection_->start();
      new_connection_.reset(new connection(io_service_, request_handler_));
      acceptor_.async_accept (new_connection_->socket(),
                              boost::bind (&AirInvServer::handle_accept, this,
                                           boost::asio::placeholders::error));
    }
  }

}
