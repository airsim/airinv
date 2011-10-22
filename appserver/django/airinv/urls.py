from django.conf.urls.defaults import *
from django.contrib import admin
admin.autodiscover()

urlpatterns = patterns('',
    # Map ^airinv to airinv/api/urls.py (airinv.api.urls module)
    (r'^sim/airinv/api/', include ('airinv.api.urls')),

    # Uncomment the admin/doc line below to enable admin documentation:
    (r'^sim/airinv/admin/doc/', include ('django.contrib.admindocs.urls')),

    # Uncomment the next line to enable the admin:
    (r'^sim/airinv/admin/', include (admin.site.urls)),
)

