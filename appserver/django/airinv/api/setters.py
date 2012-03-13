from piston.handler import BaseHandler
import json
import zmq

# The first time a request is handled by Django (after that latter has been
# started), a connection to the AirInv server is made (thanks to ZeroMQ).
zeroMQConnectionString = 'tcp://localhost:5555'
print 'Connecting to the AirInv server ('', zeroMQConnectionString, '')...'
context = zmq.Context()
socket = context.socket (zmq.REQ)
socket.connect (zeroMQConnectionString)
print '... done'

#
class SetBreakPoints (BaseHandler):	
	def read (self, request, bpJsonString):

		# Compose a JSON Python object 
		jsonAction = json.dumps (bpJsonString)
		print 'JSON serialised request: ', jsonAction

		# print 'Sending request ', request, '...'
		socket.send_unicode (jsonAction)

		# Receive the response from the airinv server,
		# formatted in JSON as well
		jsonMessage = socket.recv()
		message = json.loads (jsonMessage)	
		
		
		print 'Received reply ', request, '[', jsonMessage, ']'	
		return message
		print 'Message: ', message

	
