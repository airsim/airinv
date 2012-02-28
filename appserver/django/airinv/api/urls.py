from django.conf.urls.defaults import *
from piston.resource import Resource
from api.handlers import AirInvHandler
from api.getters import AirlineGetter

class CsrfExemptResource (Resource):
	"""Work around for the Cross-site request forgery (CSRF) issue

	See http://en.wikipedia.org/wiki/Cross-site_request_forgery.
	It is surely a bad idea to de-activate it for all the methods.
	See http://docs.djangoproject.com/en/dev/ref/contrib/csrf/
	"""
	def __init__ (self, handler, authentication = None):
		super (CsrfExemptResource, self).__init__ (handler, authentication)
		self.csrf_exempt = getattr (self.handler, 'csrf_exempt', True)

# 
display_resource = CsrfExemptResource (AirInvHandler)
get_airline_list = CsrfExemptResource (AirlineGetter)
#

urlpatterns = patterns ('',
	(r'^display/inv(?:/(?P<airlineCodeURL>\w{2,3}))?(?:/(?P<flightNumberURL>\d{1,4}))?(?:/(?P<departureDateURL>\d{2,4}-\w{3}-\d{1,2}))?$', display_resource),
	(r'^get/getairlinelist/$', get_airline_list),
)

