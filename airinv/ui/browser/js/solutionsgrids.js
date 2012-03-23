Ext.require([
    'Ext.grid.*',
    'Ext.data.*',
    'Ext.util.*',
    'Ext.state.*'
]);

Ext.onReady(function() {
	Ext.QuickTips.init();
    Ext.state.Manager.setProvider(Ext.create('Ext.state.CookieProvider'));
	
    var bkgRequest = [
						['20-Mar-11, 08:15:37','NYC','IEV','Business','3','NYC','In/Off', 2000,'20-Mar-11','60 days','20-Jun-11','30-Jun-11','10 days','25-Apr-11','8AM','10PM','BA, AA']
					 ];
	
	var solList = [
					  ['false', 'AF10/JFK-CDG/H, AF52/CDG-KBP/K', '20-Jun-11', 'AF53/KBP-CDG/K, AF11/CDG-JFK/H', '30-Jun-11', 800],
					  ['true', 'AA56/JFK-KBP/Y', '20-Jun-11', 'AA57/KBP-JFK/Y','30-Jun-11', 700],
					  ['false', 'SV5/JFK-KBP/Q', '20-Jun-11', 'SV6/KBP-JFK/Q','30-Jun-11', 800]
				  ];
			
	var bkgStore = Ext.create('Ext.data.ArrayStore', {
    	fields: [
           {name: 'BkgDelta', type:'String'},
           {name: 'Orig', type:'String'},
           {name: 'Dest', type:'String'},
		   {name: 'Cabin', type:'String'},
		   {name: 'PSize', type:'String'},
		   {name: 'POS', type:'String'},
		   {name: 'Channel', type:'String'},
		   {name: 'WTP', type:'String'},
		   {name: 'BkgDate', type:'String'},
		   {name: 'AdvPur', type:'String'},
		   {name: 'DDate', type:'String'},
		   {name: 'RDate', type:'String'},
		   {name: 'Length', type:'String'},
		   {name: 'WCancel', type:'String'},
		   {name: 'PDT', type:'String'},
		   {name: 'PRT', type:'String'},
		   {name: 'PCarrier', type:'String'}
        ],
    	    data: bkgRequest
		});			
			
	var bkgGrid = Ext.create('Ext.grid.Panel', {
        store: bkgStore,
		stateful: true,
        columns: [
            {'header': "Time-Stamp", 'width': 115, 'dataIndex': 'BkgDelta'},
            {'header': "ORG", 'width': 40, 'dataIndex': 'Orig'},
            {'header': "DES", 'width': 40, 'dataIndex': 'Dest'},
			{'header': "CAB", 'width': 60, 'dataIndex': 'Cabin'},
			{'header': "Pax #", 'width': 50, 'dataIndex': 'PSize'},
			{'header': "POS", 'width': 40, 'dataIndex': 'POS'},
			{'header': "CHA", 'width': 50, 'dataIndex': 'Channel'},
			{'header': "WTP", 'width': 65, 'dataIndex': 'WTP', 'renderer' : 'usMoney'}, //currency format
			{'header': "Bkg. Date", 'width': 70, 'dataIndex': 'BkgDate'},
			{'header': "Adv. Purchase", 'width': 80, 'dataIndex': 'AdvPur'},
			{'header': "Dep. Date", 'width': 70, 'dataIndex': 'DDate'},
			{'header': "Return Date", 'width': 70, 'dataIndex': 'RDate'},
			{'header': "Len. of Stay", 'width': 70, 'dataIndex': 'Length'},
			{'header': "Cancel?", 'width': 70, 'dataIndex': 'WCancel'},
			{'header': "Pref. Dep. Time", 'width': 100, 'dataIndex': 'PDT'},
			{'header': "Pref. Return Time", 'width': 100, 'dataIndex': 'PRT'},
			{'header': "Pref. Carriers", 'width': 80, 'dataIndex': 'PCarrier'},
			
        ],
		renderTo:'bkgRequest',
		
        width:1168,
        height:150,
		viewConfig: {
            stripeRows: true
        }
		});
	
	
	var solStore = Ext.create('Ext.data.ArrayStore', {
        fields: [
		   {name: 'selected', type:'boolean'},
           {name: 'outboundpath'},
		   {name: 'DDate'},
           {name: 'inboundpath'},
		   {name: 'RDate'},
           {name: 'Price'}
        ],
        	data: solList
		});	
		
		
		function selectionColor(val) {
        if (val == true) {
            return '<span style="color:green;">' + val + '</span>';
        } else if (val == false) {
            return '<span style="color:red;">' + val + '</span>';
        }
        	return val;
    	}
	
		var solGrid = Ext.create('Ext.grid.Panel', {
        store: solStore,
		stateful: true,
        columns: [
			{'header': "Selected", 'width': 70, 'dataIndex': 'selected', renderer:selectionColor},
            {'header': "Outbound Path", 'width': 200, 'dataIndex': 'outboundpath'},
			{'header': "Dep. Date", 'width': 120, 'dataIndex': 'DDate'},
            {'header': "Inbound Path", 'width': 200, 'dataIndex': 'inboundpath'},
			{'header': "Ret. Date", 'width': 100, 'dataIndex': 'RDate'},
            {'header': "Price", 'width': 100, 'dataIndex': 'Price', 'renderer' : 'usMoney'}  
        ],
		renderTo:'solList',
        width:1168,
        height:150,
		viewConfig: {
            stripeRows: true
        }
		});
	});
