// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <string>
#include <iostream>
#include <unistd.h>
// ZeroMQ
#include <zmq.hpp>
// AirInvServer
#include <airinv/server/BomPropertyTree.hpp>

// Receive 0MQ string from socket and convert into string
static std::string s_recv (zmq::socket_t& socket) {
  zmq::message_t message;
  socket.recv (&message);

  return std::string (static_cast<char*> (message.data()), message.size());
}

//  Convert string to 0MQ string and send to socket
static bool s_send (zmq::socket_t& socket, const std::string& string) {
  zmq::message_t message (string.size());
  memcpy (message.data(), string.data(), string.size());

  bool rc = socket.send (message);
  return rc;
}

// //////////////// M A I N ////////////////////////
int main (int argc, char* argv[]) {

  // Prepare our context and socket
  zmq::context_t context (1);
  zmq::socket_t socket (context, ZMQ_REP);
  socket.bind ("tcp://*:5555");

  while (true) {

    // Wait for next request from client
    const std::string& request = s_recv (socket);
    std::cout << "Received: '" << request << "'" << std::endl;

    // Retrieve the details for the given flight-date
    stdair::BomPropertyTree lRequestBomTree;
    lRequestBomTree.load (request);
    std::cout << "i.e., airline code = '" << lRequestBomTree._airlineCode
              << "', flight number = " << lRequestBomTree._flightNumber
              << "', departure date = '" << lRequestBomTree._departureDate
              << "'" << std::endl;

    // Send reply back to client
    stdair::BomPropertyTree lBomTree;
    lBomTree._airlineCode = "SV";
    lBomTree._flightNumber = 5;
    lBomTree._departureDate = stdair::Date_T (2010, 03, 10);
    lBomTree._airportCodeList.insert ("CDG");
    lBomTree._airportCodeList.insert ("KBP");
    const std::string& lJSONisedBomTree = lBomTree.save();
    std::cout << "Send: '" << lJSONisedBomTree << "'" << std::endl;
    s_send (socket, lJSONisedBomTree);
  }

  return 0;
}

