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

//  Convert string to 0MQ string and send to socket
static bool s_send (zmq::socket_t& socket, const std::string& string) {

  zmq::message_t message (string.size());
  memcpy (message.data(), string.data(), string.size());

  bool rc = socket.send(message);
  return (rc);
}

// //////////////// M A I N ////////////////////////
int main (int argc, char* argv[]) {

  // Prepare our context and socket
  zmq::context_t context (1);
  zmq::socket_t socket (context, ZMQ_REP);
  socket.bind ("tcp://*:5555");

  while (true) {
    zmq::message_t request;

    // Wait for next request from client
    socket.recv (&request);
    std::cout << "Received Hello" << std::endl;

    // Do some 'work'
    sleep (1);

    // Send reply back to client
    stdair::BomPropertyTree lBomTree;
    lBomTree._airlineCode = "SV";
    lBomTree._flightNumber = 5;
    lBomTree._airportCodeList.insert ("CDG");
    lBomTree._airportCodeList.insert ("KBP");
    const std::string& lJSONisedBomTree = lBomTree.save();
    s_send (socket, lJSONisedBomTree);
  }

  return 0;
}

