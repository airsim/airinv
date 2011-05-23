from django.conf.urls.defaults import *
from piston.resource import Resource
from api.handlers import AirInvHandler

class CsrfExemptResource( Resource ):
	def __init__( self, handler, authentication = None ):
		super( CsrfExemptResource, self ).__init__( handler, authentication )
		self.csrf_exempt = getattr( self.handler, 'csrf_exempt', True )

airinv_resource = CsrfExemptResource( AirInvHandler )

urlpatterns = patterns( '',
	url( r'^airinv/(?P<airlineCode>[\w]+)/(?P<flightNumber>\d+)/(?P<departureDate>[-\d\w]+)$', airinv_resource )
)

