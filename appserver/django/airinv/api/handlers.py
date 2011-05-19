from piston.handler import BaseHandler
import json
import zmq

class AirInvHandler( BaseHandler ):
	def read( self, request, expression ):
		# Extract the requested action
		fullPath = request.get_full_path()
		fullPathList = fullPath.split('/')
		actionRequested = fullPathList[-1]
		actionDetails = actionRequested.split('+')
		jsonAction = json.dumps ({'flight_date': { 'departure_date': actionDetails[2], 'airline_code': actionDetails[0], 'flight_number': actionDetails[1] }})
		# print "JSON serialised request: ", jsonAction

		# print "Connecting to the airinv server..."
		context = zmq.Context()
		socket = context.socket (zmq.REQ)
		socket.connect ("tcp://localhost:5555")

		# print "Sending request ", request, "..."
		socket.send_unicode (jsonAction)

		# Receive the response from the airinv server, formatted in JSON as well
		jsonMessage = socket.recv()
		message = json.loads (jsonMessage)

		# print "Received reply ", request, "[", jsonMessage, "]"
		return message

