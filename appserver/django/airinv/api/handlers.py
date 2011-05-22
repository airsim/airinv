from piston.handler import BaseHandler
import json
import zmq

class AirInvHandler( BaseHandler ):	
	def read( self, request, airlineCode, flightNumber, departureDate ):
		# Extract the requested action
		jsonAction = json.dumps ({'flight_date': { 'departure_date': departureDate, 'airline_code': airlineCode, 'flight_number': flightNumber }})
		# print "JSON serialised request: ", jsonAction

		# print "Connecting to the airinv server..."
		context = zmq.Context()
		socket = context.socket (zmq.REQ)
		socket.connect ("tcp://localhost:5555")

		# print "Sending request ", request, "..."
		socket.send_unicode (jsonAction)

		# Receive the response from the airinv server,
		# formatted in JSON as well
		jsonMessage = socket.recv()
		message = json.loads (jsonMessage)

		# print "Received reply ", request, "[", jsonMessage, "]"
		return message

