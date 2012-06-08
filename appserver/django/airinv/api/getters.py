from piston.handler import BaseHandler
from django.http import HttpResponse
from decorator import decorator
import json
import simplejson
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
class AirlineGetter (BaseHandler):	
	def read (self, request, callback):

		# Compose a JSON Python object 
		jsonAction = json.dumps ({"list":{"airline_code": "all", "flight_number": "0" }})
		print 'JSON serialised request: ', jsonAction

		# print 'Sending request ', request, '...'
		socket.send_unicode (jsonAction)

		# Receive the response from the airinv server,
		# formatted in JSON as well
		jsonMessage = socket.recv()
		message = json.loads (jsonMessage)	
		
		
		print 'Received reply ', request, '[', jsonMessage, ']'
		
		message = simplejson.dumps (message)
		message = callback + '(' + message + ');'	
		return HttpResponse(message, mimetype="application/json")
		print 'Message: ', message

class EventsGetter (BaseHandler):	
	def read (self, request, callback):

		# Compose a JSON Python object 
		jsonAction = json.dumps ({"event_list":{"event_type":"BookingRequest"}})
		print 'JSON serialised request: ', jsonAction

		# print 'Sending request ', request, '...'
		socket.send_unicode (jsonAction)

		# Receive the response from the airinv server,
		# formatted in JSON as well
		jsonMessage = socket.recv()
		message = json.loads (jsonMessage)	
		
		
		print 'Received reply ', request, '[', jsonMessage, ']'
		message = simplejson.dumps (message)
		message = callback + '(' + message + ');'	
		return HttpResponse(message, mimetype="application/json")
		print 'Message: ', message
		
class BreakPointsGetter (BaseHandler):	
	def read (self, request, callback):

		# Compose a JSON Python object 
		jsonAction = json.dumps ({"event_list":{"event_type":"BreakPoint"}})
		print 'JSON serialised request: ', jsonAction

		# print 'Sending request ', request, '...'
		socket.send_unicode (jsonAction)

		# Receive the response from the airinv server,
		# formatted in JSON as well
		jsonMessage = socket.recv()
		message = json.loads (jsonMessage)	
		
		
		print 'Received reply ', request, '[', jsonMessage, ']'
		message = simplejson.dumps (message)
		message = callback + '(' + message + ');'	
		return HttpResponse(message, mimetype="application/json")
		print 'Message: ', message

	
