from piston.handler import BaseHandler
from django.http import HttpResponse
from decorator import decorator
import json
import zmq
import simplejson

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
	def read (self, request, bpJsonString, callback):
				
		bpJsonString = bpJsonString.replace('_cbo', '{')
		bpJsonString = bpJsonString.replace('_cbc', '}')
		bpJsonString = bpJsonString.replace('_bbo', '[')
		bpJsonString = bpJsonString.replace('_bbc', ']')
		bpJsonString = bpJsonString.replace('_q', '"')
		bpJsonString = bpJsonString.replace('_sp', ' ')
		bpJsonString = bpJsonString.replace('_sc', ':')
		bpJsonString = bpJsonString.replace('_mn', '-')
		bpJsonString = bpJsonString.replace('_co', ',')
		
		print "Breakpoint String: " + bpJsonString
		
		try:
			bpJsonString = simplejson.loads(bpJsonString)
		except:
			print "Cannot load JSON String"
		
		#print "Breakpoint Request: " + request
		
		# Compose a JSON Python object 
		jsonAction = json.dumps (bpJsonString)
		print 'JSON serialised request: ', jsonAction

		# print 'Sending request ', request, '...'
		socket.send_unicode (jsonAction)

		# Receive the response from the airinv server,
		# formatted in JSON as well
		jsonMessage = socket.recv()
		message = json.loads (jsonMessage)	
		
		
		#print 'Received reply ', request, '[', jsonMessage, ']'
		message = simplejson.dumps (message)
		message = callback + '(' + message + ');'	
		return HttpResponse(message, mimetype="application/json")
		print 'Message: ', message

	
