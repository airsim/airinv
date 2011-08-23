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
		# print 'JSON serialised request: ', jsonAction

		# print 'Sending request ', request, '...'
		socket.send_unicode (jsonAction)

		# Receive the response from the airinv server,
		# formatted in JSON as well
		jsonMessage = socket.recv()
		message = json.loads (jsonMessage)	
		
		
		# print 'Received reply ', request, '[', jsonMessage, ']'
		#TESTING --- UNCOMMENT FOR NORMAL OPERATION ---		
		#return message
		print 'Message: ', message
		#REMOVE OR COMMENT FOR NORMAL OPERATION ---
		return {"flight_date":{"departure_date":"2010-Mar-11","airline_code":"SV","flight_number":"5","legs": {"board_date":"2010-Mar-11","capacity":"0","off_date":"2010-Mar-11","distance":"0","off_point":"JFK","board_time":"14:10:00","elapsed_time":"00:00:00","date_offset":"0","off_time":"15:55:00","board_point":"KBP","time_offset":"01:45:00"},"subclasses":[{"flight":"SV5 2010-Mar-11","segment":"KBP-JFK 2010-Mar-11","cabin":"J","ff": "1","subclass":"Y","min/au":"80(5)","nego":"0","ns%":"0","ob%":"0","bkgs":"0","grpbks":"0 (0)","stfbkgs":"0","wlbkgs":"0","etb":"0","classavl":"0","revavl":"0","segavl":"0"},{"flight":"SV5 2010-Mar-11","segment":"KBP-JFK 2010-Mar-11","cabin":"J","ff": "1","subclass":"B","min/au":"75(5)","nego":"0","ns%":"0","ob%":"0","bkgs":"0","grpbks":"0 (0)","stfbkgs":"0","wlbkgs":"0","etb":"0","classavl":"0","revavl":"0","segavl":"0"},{"flight":"SV5 2010-Mar-11","segment":"KBP-JFK 2010-Mar-11","cabin":"J","ff": "1","subclass":"M","min/au":"70(5)","nego":"0","ns%":"0","ob%":"0","bkgs":"0","grpbks":"0 (0)","stfbkgs":"0","wlbkgs":"0","etb":"0","classavl":"0","revavl":"0","segavl":"0"},{"flight":"SV5 2010-Mar-11","segment":"KBP-JFK 2010-Mar-11","cabin":"J","ff": "1","subclass":"H","min/au":"65(5)","nego":"0","ns%":"0","ob%":"0","bkgs":"0","grpbks":"0 (0)","stfbkgs":"0","wlbkgs":"0","etb":"0","classavl":"0","revavl":"0","segavl":"0"},{"flight":"SV5 2010-Mar-11","segment":"KBP-JFK 2010-Mar-11","cabin":"J","ff": "1","subclass":"X","min/au":"60(5)","nego":"0","ns%":"0","ob%":"0","bkgs":"0","grpbks":"0 (0)","stfbkgs":"0","wlbkgs":"0","etb":"0","classavl":"0","revavl":"0","segavl":"0"},{"flight":"SV5 2010-Mar-11","segment":"KBP-JFK 2010-Mar-11","cabin":"J","ff": "1","subclass":"Q","min/au":"55(5)","nego":"0","ns%":"0","ob%":"0","bkgs":"0","grpbks":"0 (0)","stfbkgs":"0","wlbkgs":"0","etb":"0","classavl":"0","revavl":"0","segavl":"0"},{"flight":"SV5 2010-Mar-11","segment":"KBP-JFK 2010-Mar-11","cabin":"J","ff": "1","subclass":"N","min/au":"50(5)","nego":"0","ns%":"0","ob%":"0","bkgs":"0","grpbks":"0 (0)","stfbkgs":"0","wlbkgs":"0","etb":"0","classavl":"0","revavl":"0","segavl":"0"},{"flight":"SV5 2010-Mar-11","segment":"KBP-JFK 2010-Mar-11","cabin":"J","ff": "1","subclass":"V","min/au":"45(5)","nego":"0","ns%":"0","ob%":"0","bkgs":"0","grpbks":"0 (0)","stfbkgs":"0","wlbkgs":"0","etb":"0","classavl":"0","revavl":"0","segavl":"0"},{"flight":"SV5 2010-Mar-11","segment":"KBP-JFK 2010-Mar-11","cabin":"J","ff": "1","subclass":"W","min/au":"40(5)","nego":"0","ns%":"0","ob%":"0","bkgs":"0","grpbks":"0 (0)","stfbkgs":"0","wlbkgs":"0","etb":"0","classavl":"0","revavl":"0","segavl":"0"},{"flight":"SV5 2010-Mar-11","segment":"KBP-JFK 2010-Mar-11","cabin":"J","ff": "1","subclass":"U","min/au":"35(5)","nego":"0","ns%":"0","ob%":"0","bkgs":"0","grpbks":"0 (0)","stfbkgs":"0","wlbkgs":"0","etb":"0","classavl":"0","revavl":"0","segavl":"0"},{"flight":"SV5 2010-Mar-11","segment":"KBP-JFK 2010-Mar-11","cabin":"J","ff": "1","subclass":"S","min/au":"30(5)","nego":"0","ns%":"0","ob%":"0","bkgs":"0","grpbks":"0 (0)","stfbkgs":"0","wlbkgs":"0","etb":"0","classavl":"0","revavl":"0","segavl":"0"},{"flight":"SV5 2010-Mar-11","segment":"KBP-JFK 2010-Mar-11","cabin":"J","ff": "1","subclass":"L","min/au":"25(5)","nego":"0","ns%":"0","ob%":"0","bkgs":"0","grpbks":"0 (0)","stfbkgs":"0","wlbkgs":"0","etb":"0","classavl":"0","revavl":"0","segavl":"0"},{"flight":"SV5 2010-Mar-11","segment":"KBP-JFK 2010-Mar-11","cabin":"J","ff": "1","subclass":"G","min/au":"20(5)","nego":"0","ns%":"0","ob%":"0","bkgs":"0","grpbks":"0 (0)","stfbkgs":"0","wlbkgs":"0","etb":"0","classavl":"0","revavl":"0","segavl":"0"},{"flight":"SV5 2010-Mar-11","segment":"KBP-JFK 2010-Mar-11","cabin":"J","ff": "1","subclass":"K","min/au":"15(5)","nego":"0","ns%":"0","ob%":"0","bkgs":"0","grpbks":"0 (0)","stfbkgs":"0","wlbkgs":"0","etb":"0","classavl":"0","revavl":"0","segavl":"0"},{"flight":"SV5 2010-Mar-11","segment":"KBP-JFK 2010-Mar-11","cabin":"J","ff": "1","subclass":"T","min/au":"10(5)","nego":"0","ns%":"0","ob%":"0","bkgs":"0","grpbks":"0 (0)","stfbkgs":"0","wlbkgs":"0","etb":"0","classavl":"0","revavl":"0","segavl":"0"},{"flight":"SV5 2010-Mar-11","segment":"KBP-JFK 2010-Mar-11","cabin":"J","ff": "1","subclass":"E","min/au":"5(5)","nego":"0","ns%":"0","ob%":"0","bkgs":"0","grpbks":"0 (0)","stfbkgs":"0","wlbkgs":"0","etb":"0","classavl":"0","revavl":"0","segavl":"0"}],"bid_price_vectors":"[2871, 2672, 2546, 2452, 2361, 2292, 2235, 2192, 2153, 2112, 2070, 2027, 1983]"}}

