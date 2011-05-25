from piston.handler import BaseHandler
import json
import zmq

# The first time a request is handled by Django (after that latter has been
# started), a connection to the AirInv server is made (thanks to ZeroMQ).
zeroMQConnectionString = "tcp://localhost:5555"
print "Connecting to the AirInv server ('", zeroMQConnectionString, "')..."
context = zmq.Context()
socket = context.socket (zmq.REQ)
socket.connect (zeroMQConnectionString)
print "... done"

#
class AirInvHandler (BaseHandler):	
	def read (self, request, airlineCodeURL=None, flightNumberURL=None, departureDateURL=None):
		# Default values, corresponding to the default inventory dump
		# file loaded by the inventory server
		airlineCode = 'SV'
		flightNumber = 5
		departureDate = '2010-Mar-11'

		# Extract the requested action from the query string (if nay)
		airlineCodeQuery = request.GET.get ('ac')
		flightNumberQuery = request.GET.get ('fn')
		departureDateQuery = request.GET.get ('dd')

		# If the flight-date parameters are given within the URL (e.g.,
		# http://localhost:8000/sim/airinv/api/airinv/SV/5/2010-Mar-11)
		# take them into account
		if airlineCodeQuery: airlineCode = airlineCodeQuery
		if flightNumberQuery: flightNumber = flightNumberQuery
		if departureDateQuery: departureDate = departureDateQuery

		# If the flight-date parameters are given as part of the query
		# string (e.g., http://localhost:8000/sim/airinv/api/airinv?ac=SV&fn=5&dd=2010-Mar-11)
		# take them into account. They have the precedence over the 
		# parameters defined within the URL. That is, with
		# http://localhost:8000/sim/airinv/api/airinv/TU/4/2011-Apr-10?ac=SV&fn=5&dd=2010-Mar-11
		# the following parameters would be kept: SV, 5, 2010-Mar-11
		if airlineCodeURL: airlineCode = airlineCodeURL
		if flightNumberURL: flightNumber = flightNumberURL
		if departureDateURL: departureDate = departureDateURL

		# Compose a JSON Python object with the parameters
		jsonAction = json.dumps ({'flight_date': { 'departure_date': departureDate, 'airline_code': airlineCode, 'flight_number': flightNumber }})
		# print "JSON serialised request: ", jsonAction

		# print "Sending request ", request, "..."
		socket.send_unicode (jsonAction)

		# Receive the response from the airinv server,
		# formatted in JSON as well
		jsonMessage = socket.recv()
		message = json.loads (jsonMessage)

		# print "Received reply ", request, "[", jsonMessage, "]"
		return message

