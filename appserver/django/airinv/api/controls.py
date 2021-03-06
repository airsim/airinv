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
class SimulatorCommands (BaseHandler):	
	def read (self, request, command, callback):
		print 'Command received: ', command
		
		if command == "run":
			jsonAction = json.dumps ({"run":"1"})
			print 'JSON serialised request: ', jsonAction

			# print 'Sending request ', request, '...'
			socket.send_unicode (jsonAction)

			jsonMessage = socket.recv()
			message = json.loads (jsonMessage)	

			print 'Received reply ', request, '[', jsonMessage, ']'	
			message = simplejson.dumps (message)
			message = callback + '(' + message + ');'	
			return HttpResponse(message, mimetype="application/json")
			print 'Message: ', message
			
		if command == "reset":
			jsonAction = json.dumps ({"reset":"1"})
			print 'JSON serialised request: ', jsonAction

			# print 'Sending request ', request, '...'
			socket.send_unicode (jsonAction)
			
			jsonMessage = socket.recv()
			message = json.loads (jsonMessage)	
			
			print 'Received reply ', request, '[', jsonMessage, ']'	
			message = simplejson.dumps (message)
			message = callback + '(' + message + ');'	
			return HttpResponse(message, mimetype="application/json")
			print 'Message: ', message
			
		if command == "status":
			jsonAction = json.dumps ({"status":"1"})
			print 'JSON serialised request: ', jsonAction

			# print 'Sending request ', request, '...'
			socket.send_unicode (jsonAction)

			jsonMessage = socket.recv()
			message = json.loads (jsonMessage)	

			print 'Received reply ', request, '[', jsonMessage, ']'
			message = simplejson.dumps (message)
			message = callback + '(' + message + ');'	
			return HttpResponse(message, mimetype="application/json")
			print 'Message: ', message
