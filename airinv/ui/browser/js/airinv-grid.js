Ext.require([
    'Ext.grid.*',
    'Ext.data.*',
    'Ext.util.*',
    'Ext.state.*'
]);



/*var grid_flight;
var grid_legs;
var grid_subclasses;
var store_flights;
var store_legs;
var store_subclasses;
*/

var idFlight;

function queryBuild(companyCode, flightNumber, date)
{
	
	//url="http://ncevsediri-fed/api/display/inv/" + companyCode + "/" + flightNumber + "/" + date;
	//url="D:/Development%20Workspace/Dreamweaver/DsimUI/browser/sample/datasample.html";
	grid_flight.getStore().load({url:url});
	grid_legs.getStore().load({url:url});
	grid_subclasses.getStore().load({url:url});
	//flightURL = queryURL;
}

//Delete this function when calling Django server
/*function tempQuery(jsonContent)
{

	store_flights = Ext.create('Ext.data.Store', {
        autoLoad: true,
        autoSync: true,
        model: 'flightinfo',
		reader: myReader
    });
	store_flights.loadData(jsonContent);
	grid_legs.getStore().loadData(jsonContent);
	grid_subclasses.getStore().loadData(jsonContent);
	//flightURL = queryURL;
}*/

Ext.onReady(function(){

	Ext.define('flightinfo', {
    extend: 'Ext.data.Model',
    fields: ['departure_date','airline_code','flight_number'],
	
	hasMany: {model: 'legsinfo', name: 'legs'}
});


Ext.define('legsinfo', {
    extend: 'Ext.data.Model',
    fields: [
	{
        name: 'board_date',
        type: 'shortdate'
    }, 
	{
		name:'capacity',
		type:'int'
	}, 
	{ 
		name:'off_date',
		type:'shortdate'
	},
	{
		name:'distance',
		type:'int'
	},
	{
		name:'off_point',
		type:'string'
	},
	{
		name:'board_time',
		type:'string'
	},
	{
		name:'elapsed_time',
		type:'string'
	},
	{
		name:'date_offset',
		type:'string'
	},
	{
		name:'off_time',
		type:'string'
	},
	{
		name:'board_point',
		type:'string'
	},
	{
		name:'time_offset',
		type:'string'
	}
	],
	
	belongsTo: 'flightinfo'
});


Ext.define('subclasses', {
    extend: 'Ext.data.Model',
    fields: [
	{
        name: 'flight',
        type: 'string'
    }, 
	{
		name:'segment',
		type:'string'
	}, 
	{ 
		name:'cabin',
		type:'string'
	},
	{
		name:'ff',
		type:'string'
	},
	{
		name:'subclass',
		type:'string'
	},
	{
		name:'min/au',
		type:'string'
	},
	{
		name:'nego',
		type:'string'
	},
	{
		name:'ns%',
		type:'string'
	},
	{
		name:'ob%',
		type:'string'
	},
	{
		name:'bkgs',
		type:'string'
	},
	{
		name:'grpbks',
		type:'string'
	},
	{
		name:'stfbkgs',
		type:'string'
	},
	{
		name:'wlbkgs',
		type:'string'
	},
	{
		name:'etb',
		type:'string'
	},
	{
		name:'classavl',
		type:'string'
	},
	{
		name:'revavl',
		type:'string'
	},
	{
		name:'segavl',
		type:'string'
	}
	],
	
	belongsTo: 'flightinfo'
});

	var url="http://ncevsediri-fed/api/display/inv/SV/5/2010-Mar-11";
    var store_flights = Ext.create('Ext.data.Store', {
        model: 'flightinfo',
        proxy: {
            type: 'rest',
			method: 'POST',
            url: url,
            reader: {
                type: 'json',
                root: 'flight_date'
            }, 
        }   
    });
	store_flights.load();
	
	var store_legs = Ext.create('Ext.data.Store', {
        model: 'legsinfo',
        proxy: {
            type: 'rest',
			method: 'POST',
            url: url,
            reader: {
                type: 'json',
                root: 'flight_date.legs'
            }, 
        }  
    });
	store_legs.load();
	
	var store_subclasses = Ext.create('Ext.data.Store', {
        model: 'subclasses',
        proxy: {
            type: 'rest',
			method: 'POST',
            url: url,
            reader: {
                type: 'json',
                root: 'flight_date.subclasses'
            }, 
        }    
    });
	store_subclasses.load();
	
	var cellEditing = Ext.create('Ext.grid.plugin.CellEditing', {
        clicksToEdit: 1
    });
	
    // create the grid
    var grid_flight = new Ext.grid.GridPanel({
        store: store_flights,
        columns: [
            {'header': "Departure Date", 'width': 120, 'dataIndex': 'departure_date', sortable: true},
            {'header': "Airline Code", 'width': 100, 'dataIndex': 'airline_code', sortable: true},
            {'header': "Flight Number", 'width': 100, 'dataIndex': 'flight_number', sortable: true}  
        ],
        title: 'Found Flights',
		renderTo:'flights-GridDisplay',
        width:320,
        height:150
		/*listeners: {
			itemclick : function() {
				var data=grid_flight.getSelectionModel().selected.items[0].data;
				grid_legs.setTitle('Legs List');
				store_legs.clearFilter();
				store_legs.load();
			}
		}*/	
    });

	
    // create the grid
    var grid_legs = new Ext.grid.GridPanel({
        store: store_legs,
        columns: [
            {header: "Board Date", width: 80, dataIndex: 'board_date', sortable: true},
            {header: "Capacity", width: 75, dataIndex: 'capacity', sortable: true},
            {header: "Off Date", width: 80, dataIndex: 'off_date', sortable: true},
            {header: "Distance", width: 80, dataIndex: 'distance', sortable: true},
            {header: "Off Point", width: 80, dataIndex: 'off_point', sortable: true},
			{header: "Board Time", width: 80, dataIndex: 'board_time', sortable: true},
			{header: "Elapsed Time", width: 80, dataIndex: 'elapsed_time', sortable: true},
			{header: "Date Offset", width: 80, dataIndex: 'date_offset', sortable: true},
			{header: "Off Time", width: 80, dataIndex: 'off_time', sortable: true},
			{header: "Board Point", width: 80, dataIndex: 'board_point', sortable: true},
			{header: "Time Offset", width: 80, dataIndex: 'time_offset', sortable: true},
			
        ],
        title: 'Flight Legs',
		renderTo:'legs-GridDisplay',
        width:875,
        height:150
    });
	
	var grid_subclasses = new Ext.grid.GridPanel({
        store: store_subclasses,
        columns: [
            {header: "Flight", width: 100, dataIndex: 'flight', sortable: true},
            {header: "Segment", width: 120, dataIndex: 'segment', sortable: true},
            {header: "Cabin", width: 50, dataIndex: 'cabin', sortable: true},
            {header: "FF", width: 30, dataIndex: 'ff', sortable: true},
            {header: "Subclass", width: 70, dataIndex: 'subclass', sortable: true},
			{header: "MIN/AU (Prot)", width: 90, dataIndex: 'min/au', sortable: true},
			{header: "Nego", width: 40, dataIndex: 'nego', sortable: true, type: 'int',field: {xtype: 'numberfield',allowBlank: false,minValue: 0,maxValue: 100}},
			{header: "NS%", width: 40, dataIndex: 'ns%', sortable: true, type: 'int',field: {xtype: 'numberfield',allowBlank: false,minValue: 0,maxValue: 100}},
			{header: "OB%", width: 40, dataIndex: 'ob%', sortable: true, type: 'int',field: {xtype: 'numberfield',allowBlank: false,minValue: 0,maxValue: 100}},
			{header: "Bookings", width: 60, dataIndex: 'bkgs', sortable: true, type: 'int',field: {xtype: 'numberfield',allowBlank: false,minValue: 0,maxValue: 100}},
			{header: "Group Bookings", width: 90, dataIndex: 'grpbks', sortable: true, type: 'int',field: {xtype: 'numberfield',allowBlank: false,minValue: 0,maxValue: 100}},
			{header: "Staff Bookings", width: 90, dataIndex: 'stfbkgs', sortable: true, type: 'int',field: {xtype: 'numberfield',allowBlank: false,minValue: 0,maxValue: 100}},
			{header: "WL Bookings", width: 80, dataIndex: 'wlbkgs', sortable: true, type: 'int',field: {xtype: 'numberfield',allowBlank: false,minValue: 0,maxValue: 100}},
			{header: "ETB", width: 40, dataIndex: 'etb', sortable: true, type: 'int',field: {xtype: 'numberfield',allowBlank: false,minValue: 0,maxValue: 100}},
			{header: "Class AVL", width: 80, dataIndex: 'classavl', sortable: true, type: 'int',field: {xtype: 'numberfield',allowBlank: false,minValue: 0,maxValue: 100}},
			{header: "Rev AVL", width: 70, dataIndex: 'revavl', sortable: true, type: 'int',field: {xtype: 'numberfield',allowBlank: false,minValue: 0,maxValue: 100}},
			{header: "Seg AVL", width: 70, dataIndex: 'segavl', sortable: true, type: 'int',field: {xtype: 'numberfield',allowBlank: false,minValue: 0,maxValue: 100}},
        ],
        title: 'Subclasses',
		renderTo:'subclasses-GridDisplay',
        width:1180,
        height:400,
		plugins: [cellEditing]
    });
}
)
/*var IMG_GO = "assets/icons/iconDeparture.png";
function renderSelect(legsArray) {
		//return Ext.String.format(
            return '<img src=' + IMG_GO + ' /><input type="button" value="Display Legs" id="bt"onClick="showLegs()"/>'
        //<a href="http://www.espn.com" target="_blank">);
		//<input type="button" value="toto" id="'+val+'"/>
    }*/
