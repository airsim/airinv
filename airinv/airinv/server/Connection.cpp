// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <vector>
// Boost
#include <boost/bind.hpp>
// AirInv
#include <airinv/server/RequestHandler.hpp>
#include <airinv/server/Connection.hpp>

namespace AIRINV {

  // //////////////////////////////////////////////////////////////////////
  Connection::Connection (boost::asio::io_service& ioService,
                          RequestHandler& ioHandler)
    : _strand (ioService), _socket (ioService), _requestHandler (ioHandler) {
  }

  // //////////////////////////////////////////////////////////////////////
  boost::asio::ip::tcp::socket& Connection::socket() {
    return _socket;
  }

  // //////////////////////////////////////////////////////////////////////
  void Connection::start() {
    
    _socket.async_read_some (boost::asio::buffer (_buffer),
                             _strand.wrap (boost::bind (&Connection::handleRead,
                                                        shared_from_this(),
                                                        boost::asio::placeholders::error,
                                                        boost::asio::placeholders::bytes_transferred)));
  }

  // //////////////////////////////////////////////////////////////////////
  void Connection::handleRead (const boost::system::error_code& iErrorCode,
                               std::size_t bytes_transferred) {
    if (!iErrorCode) {
      
      boost::tribool result;
      boost::tie (result, boost::tuples::ignore) =
        _requestParser.parse (_request, _buffer.data(),
                              _buffer.data() + bytes_transferred);
        
      if (result) {
        _requestHandler.handleRequest (_request, _reply);
        
        boost::asio::async_write (_socket, _reply.to_buffers(),
                                  _strand.wrap (boost::bind (&Connection::handleWrite,
                                                             shared_from_this(),
                                                             boost::asio::placeholders::error)));
        
      } else if (!result) {
        
        _reply = Reply::stock_reply (Reply::bad_request);
        boost::asio::async_write (_socket, _reply.to_buffers(),
                                  _strand.wrap (boost::bind (&Connection::handleWrite,
                                                             shared_from_this(),
                                                             boost::asio::placeholders::error)));
        
      } else {
        
        _socket.async_read_some (boost::asio::buffer (_buffer),
                                 _strand.wrap (boost::bind (&Connection::handleRead,
                                                            shared_from_this(),
                                                            boost::asio::placeholders::error,
                                                            boost::asio::placeholders::bytes_transferred)));
      }
    }

    // If an error occurs then no new asynchronous operations are
    // started. This means that all shared_ptr references to the
    // connection object will disappear and the object will be
    // destroyed automatically after this handler returns. The
    // connection class's destructor closes the socket.
  }

  // //////////////////////////////////////////////////////////////////////
  void Connection::handleWrite (const boost::system::error_code& iErrorCode) {
    
    if (!iErrorCode) {
      // Initiate graceful connection closure.
      boost::system::error_code ignored_ec;
      _socket.shutdown (boost::asio::ip::tcp::socket::shutdown_both,
                        ignored_ec);
    }

    // No new asynchronous operations are started. This means that all
    // shared_ptr references to the connection object will disappear
    // and the object will be destroyed automatically after this
    // handler returns. The connection class's destructor closes the
    // socket.
  }

}
