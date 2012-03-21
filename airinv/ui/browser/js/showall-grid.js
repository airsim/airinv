Ext.require([
    'Ext.grid.*',
    'Ext.data.*',
    'Ext.util.*',
    'Ext.state.*'
]);

Ext.define('airlineinfo', {
	extend: 'Ext.data.Model',
	fields: [
	{name:'airline_code', type: 'string'}, 
	{name:'date', type:'string'}, 
	{name:'number', type: 'int'},
	]	
});



Ext.QuickTips.init();
Ext.state.Manager.setProvider(Ext.create('Ext.state.CookieProvider'));

var url = "http://ncevsediri-fed/api/get/getairlinelist/";


function showAllGrid()
{

	/*
	var bkgRequest = [
						['20-Mar-11, 08:15:37','NYC','IEV','Business','3','NYC','In/Off', 2000,'20-Mar-11','60 days','20-Jun-11','30-Jun-11','10 days','25-Apr-11','8AM','10PM','BA, AA']
					 ];
	
	var solList = [
					  ['false', 'AF10/JFK-CDG/H, AF52/CDG-KBP/K', '20-Jun-11', 'AF53/KBP-CDG/K, AF11/CDG-JFK/H', '30-Jun-11', 800],
					  ['true', 'AA56/JFK-KBP/Y', '20-Jun-11', 'AA57/KBP-JFK/Y','30-Jun-11', 700],
					  ['false', 'SV5/JFK-KBP/Q', '20-Jun-11', 'SV6/KBP-JFK/Q','30-Jun-11', 800]
				  ];
	*/
	
	var store_airlines = Ext.create('Ext.data.Store', {
		model: 'airlineinfo',
		proxy: {
			type: 'rest',
			method: 'POST',
			url: url,
			reader: {
				type: 'json',
				root: 'inventories'
			}, 
		}   
	});
	store_airlines.load();
	
	var bkgGrid = Ext.create('Ext.grid.Panel', {
		store: store_airlines,
		stateful: true,
		columns: [
			{'header': "Airline Code", 'width': 70, 'dataIndex': 'airline_code'},
			{'header': "Flight Number", 'width': 80, 'dataIndex': 'number'},
			{'header': "Date", 'width': 80, 'dataIndex': 'date'},		
		],
		renderTo:'allinvgrid',
		
		width:250,
		height:400,
		viewConfig: {
			stripeRows: true
		}
		});
	

}

