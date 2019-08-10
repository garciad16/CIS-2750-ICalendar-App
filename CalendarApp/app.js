'use strict'

//app.js contains the express framework to handle the server side javascript endpoints for the main functionality 
//and calling the ffi library, the ffi library calls the C library 

// C library API
const ffi = require('ffi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');
const mysql = require('mysql');

//TODO - specify the pointer
var ref = require('ref');

app.use(fileUpload());

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }

  let uploadFile = req.files.uploadFile;
  
  if(uploadFile){
	 console.log('hitting upload function' + uploadFile.name);
	 
	// Use the mv() method to place the file somewhere on your server
	uploadFile.mv('uploads/' + uploadFile.name, function(err) {
		if(err) {
			return res.status(500).send(err);
		}
		res.redirect('/');
	});

  }

});

//Respond to GET requests for files in the uploads/ directory
//:name is the file name, any uploads + file name url will call this function
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    console.log(err);
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** //
//*** Set up a function from the C math library ***//
var libm = ffi.Library('libm', {
  'ceil': [ 'double', [ 'double' ] ]
});

//Call a function from the C math library
let c = libm.ceil(1.5); // 2
console.log("ffiTest.js: libm.ceil(1.5) returned "+c+"\n");


// typedefs
var myobj = ref.types.void // we don't know what the layout of "myobj" looks like
var myobjPtr = ref.refType(myobj);

//Creates a ctype object for the given ct
//ctype = ffi.typeof(ct)

/**** Set up functions from our shared library ****

We create a new object called testA3 and the C functions become its methods

*/
let testA3 = ffi.Library('./sharedLib', {
	//return type first, argument list second
	//for void input type, leave argument list empty
	//int return, int argument
  'calendarToJSON' : [ 'string', [] ],
  'calOpen' : [ 'string', ['string'] ],
  'calOpenEvent' : [ 'string', ['string'] ],
  'newCal' : [ 'string', ['string', 'string', 'string', 'string'] ],
  'newEvent' : [ 'string', ['string', 'string', 'string'] ],
  'calOpenOptionalEvent' : [ 'string', ['string'] ],
});

app.get('/getallfilenames', function(req , res){	
	
	var uploadFolderName = './uploads/';
	var arrFileNames = new Array();
	let calArr = new Array();
	//let objCal = {};

	fs.readdir(uploadFolderName, (err, files) => 
	{
		if (err) {
			console.error(err);
		} else {
			console.log('Directory is:', files);
		}
	  files.forEach(file => {
		//console.log('file: ' + file);
		arrFileNames.push(file);
		
		//let descCal = testA3.calOpen(file);
		let fileInfo = testA3.calOpen(uploadFolderName + file);
		let calData = JSON.parse(fileInfo);
		let JSONFile = JSON.stringify(calData);
		if (calData){
			console.log("ffiTest.js: calOpen() returned "+calData.version+"\n");
		}
		else{
			console.log("calData doesn't Exist\n");
		}
		let objCal = {};
		objCal.version = calData.version;
		objCal.prodID = calData.prodID;
		objCal.numProps = calData.numProps;
		objCal.numEvents = calData.numEvents;
		console.log("objCal return" + objCal+ "\n");

		calArr.push(objCal);

	  });
	  
	  //TODO get the file data here for C, because right now, only the filenames are being passed
	  
	  res.send({ 
		fileNames: arrFileNames,
		calNames: calArr,
	   });
	});
	
	console.log('test');

});

app.get('/getallfilenames2', function(req, res){
	var uploadFolderName2 = './uploads/';
	//var arrFileNames2 = new Array();
	let calArr = new Array();


	var fileNameDir = './uploads/'+req.query.name;
	console.log('fileNameDir : ' + fileNameDir);
		/*if(err){
			console.error(err);
		}
		else{
			console.log('Directory is the following:', files);
		}*/
			//arrFileNames2.push(file);

			let fileInfo = testA3.calOpen(fileNameDir);
			let calData = JSON.parse(fileInfo);
			//let JSONFile = JSON.parse(calData);
			if (calData){
				console.log("If calData is successful" +calData.version+"\n");
			}
			else{
				console.log("CalData doesn't exist\n");
			}
			let objCal2 = {}
			objCal2.version = calData.version;
			objCal2.prodID = calData.prodID;
			objCal2.numProps = calData.numProps;
			objCal2.numEvents = calData.numEvents;

			calArr.push(objCal2);

			console.log("This is objCal2" + objCal2);

		res.send({
			calNames: objCal2,
		});
});

app.get('/getallfilenames3', function(req, res){
	var uploadFolderName = './uploads/';
	var arrFileNames = new Array();
	let evtArr = new Array();
	let arrDT = new Array();
	//let objCal = {};

	var fileNameDir = './uploads/'+req.query.name;
	console.log('fileNameDir : ' + fileNameDir);
		
		//let descCal = testA3.calOpen(file);
		let fileInfo = testA3.calOpenEvent(fileNameDir);
		let evtData = JSON.parse(fileInfo);
		let JSONFile = JSON.stringify(evtData);
		if (evtData){
			console.log("EventList data contains "+JSONFile+"\n");
		}
		else{
			console.log("calData doesn't Exist\n");
		}
		
		
		let evt = 1;
		for(let evtList of evtData){
			console.log("evtList example is:" + evtList.startDT);
			arrDT.push(evtList.startDT);
			let objEvt = {};
			objEvt.numProps = evtList.numProps;
			objEvt.numAlarms = evtList.numAlarms;
			objEvt.summary = evtList.summary;
			console.log("objEvt.numAlarms return" + objEvt.numAlarms+ "\n");

			evtArr.push(objEvt);
			
			evt += 1;
		}
		
		let dtJSON = JSON.stringify(arrDT);
		
		console.log("evt Data DT example is:" + dtJSON);
		
	  
	  //TODO get the file data here for C, because right now, only the filenames are being passed
	  
	  res.send({ 
		//fileNames: arrFileNames,
		dtNames: arrDT,
		evtNames : evtArr,
	   });
	
});

app.get('/newCalendar', function(req, res){
  
	  console.log('req2: ' + req.query.calData);
	  
	  if(req.query.calData)
	  {
		  var parsedObj = JSON.parse(req.query.calData);	  
		  var file = parsedObj.file;
		  
		  //console.log("checking value of:" + parsedObj.calJSON);
		  
		  var calJSON = JSON.stringify(parsedObj.calJSON);		  	  
		  var uidJSON = JSON.stringify(parsedObj.uidJSON);		  
		  var dtJSON = JSON.stringify(parsedObj.dtJSON);		  
		  var fileNameDir = './uploads/'+file;
		  
		  console.log('file: ' + fileNameDir);
		  //console.log('calJSON: ' + calJSON);
		  console.log('uidJSON: ' + uidJSON);
		  console.log('dtJSON: ' + dtJSON);
		  
		  let fileReturn = testA3.newCal(fileNameDir, calJSON, uidJSON, dtJSON);
		  console.log("newCal returns:" + fileReturn);
		  
		  res.send({ 
			 result:fileReturn
		   });
      }
});

app.get('/newEvent', function(req, res){
  
	  console.log('req2: ' + req.query.calData);
	  
	  if(req.query.calData)
	  {
		  //if JSON.parse it will turn from string to obj
		  var parsedObj = JSON.parse(req.query.calData);		  
		  var file = parsedObj.file;		  
		  
		  //if JSON.stringify it turn from obj to string
		  var uidJSON = JSON.stringify(parsedObj.uidJSON);		  
		  var dtJSON = JSON.stringify(parsedObj.dtJSON);		  
		  var fileNameDir = './uploads/'+ file;
		  
		  console.log('file: ' + fileNameDir);
		  console.log('uidJSON: ' + uidJSON);
		  console.log('dtJSON: ' + dtJSON);
		  
		  let fileReturn = testA3.newEvent(fileNameDir, uidJSON, dtJSON);
		  // fileReturn returns a string and to send it to index.js, do res.send as an object
		  console.log(fileReturn);
		  
		  res.send({ 
			 result:fileReturn
		   });
      }  
	
});

app.get('/getFiles', function(req , res){
	
	
	var uploadFolderName = './uploads/';
	var arrFileNames = new Array();
	let calArr = new Array();
	//let objCal = {};

	fs.readdir(uploadFolderName, (err, files) => 
	{
		if (err) {
			console.error(err);
		} else {
			console.log('Directory is:', files);
		}
	  files.forEach(file => {
		//console.log('file: ' + file);
		arrFileNames.push(file);
		
		//let descCal = testA3.calOpen(file);
		let fileInfo = testA3.calOpen(uploadFolderName + file);
		let calData = JSON.parse(fileInfo);
		let JSONFile = JSON.stringify(calData);
		if (calData){
			console.log("ffiTest.js: calOpen() returned "+calData.version+"\n");
		}
		else{
			console.log("calData doesn't Exist\n");
		}
		let objCal = {};
		objCal.version = calData.version;
		objCal.prodID = calData.prodID;
		objCal.numProps = calData.numProps;
		objCal.numEvents = calData.numEvents;
		console.log("objCal return" + objCal+ "\n");

		calArr.push(objCal);

	  });
	  
	  //TODO get the file data here for C, because right now, only the filenames are being passed
	  
	  res.send({ 
		fileNames: arrFileNames,
		calNames: calArr,
	   });
	});
	
	console.log('test');

});

app.get('/getOptionalEvents', function(req , res){
	
	var uploadFolderName = './uploads/';
	var arrFileNames = new Array();
	let evtOptArr = new Array();
	//let objCal = {};

	fs.readdir(uploadFolderName, (err, files) => 
	{
		if (err) {
			console.error(err);
		} else {
			console.log('Directory is:', files);
		}
	  files.forEach(file => {
		//console.log('file: ' + file);
		arrFileNames.push(file);
		
		let fileInfo = testA3.calOpenOptionalEvent(uploadFolderName + file);
		let optionEvtData = JSON.parse(fileInfo);
		if (optionEvtData){
			console.log("optional events string is  "+optionEvtData+"\n");
		}
		else{
			console.log("calData doesn't Exist\n");
		}
		
		let optEvt = {};
		optEvt.organizer = optionEvtData.organizer;
		optEvt.location = optionEvtData.location;
		
		evtOptArr.push(optEvt);

	  });
	  
	  //TODO get the file data here for C, because right now, only the filenames are being passed
	  
	  res.send({ 
			optProp: evtOptArr
	   });
	});
	
	console.log('test');

});

var connection;

app.get('/login', function(req, res) {
	var parsedLogin = JSON.parse(req.query.data);
	var userDB = parsedLogin.userName;
	var passDB = parsedLogin.password;
	var dbName = parsedLogin.database;

	console.log('database password obj: ' + parsedLogin);
	console.log('database password is: ' + dbName);
	
	try{
		connection = mysql.createConnection({
			host     : 'dursley.socs.uoguelph.ca',
			user     : userDB,
			password : passDB,
			database : dbName
		});

		connection.connect(function(err) {
		  if (err){
			  res.send({ 
						connected: false
				   });
			console.log("Can't connect!");
		  }
		  else{
			  //if (err) throw err;
			  console.log("Connected!");
			  console.log('state: ' + connection.state);
			  
			    var fileTable = "CREATE TABLE IF NOT EXISTS FILE (cal_id INT AUTO_INCREMENT, file_Name VARCHAR(60) NOT NULL, version INT NOT NULL, prod_id VARCHAR(256) NOT NULL, PRIMARY KEY (cal_id))";
				connection.query(fileTable, function (err, rows, fields)  {
					if (err) throw err;
						console.log("File Table created");
				});
					
				var eventTable = "CREATE TABLE IF NOT EXISTS EVENT (event_id INT AUTO_INCREMENT, summary VARCHAR(1024), start_time DATETIME NOT NULL, location VARCHAR(60), organizer VARCHAR (256), cal_file INT NOT NULL, PRIMARY KEY (event_id), FOREIGN KEY (cal_file) REFERENCES FILE(cal_id) ON DELETE CASCADE)";
				connection.query(eventTable, function (err, rows, fields)  {
					if (err) throw err;
						console.log("Event Table created");
				});

				var alarmTable = "CREATE TABLE IF NOT EXISTS ALARM (alarm_id INT AUTO_INCREMENT, action VARCHAR(256) NOT NULL, `trigger` VARCHAR(256) NOT NULL, event INT NOT NULL, PRIMARY KEY (alarm_id), FOREIGN KEY (event) REFERENCES EVENT(event_id) ON DELETE CASCADE)";
				connection.query(alarmTable, function (err, rows, fields)  {
					if (err) throw err;
						console.log("Alarm Table created");
				});

				console.log('state: ' + connection.state);

				/*Comeback to this regarding disconnecting*/
				if(connection.state == 'disconnected'){
					res.send({ 
						connected: false
				   });
				}
				else{
					res.send({ 
						connected: true
				   });
				}	
		  }
		  
		});
	}
	catch(e){
			console.log('Cannot connect to server or invalid credentials');
	}


//connection.end();

});

app.get('/ClearData', function(req, res) {

if(connection){
	connection.query("DELETE FROM FILE", function (err, rows, fields) {
		if (err) 
			console.log("Something went wrong. "+err);
		else
			console.log("Cleared data from table");
	});
	
	connection.query("DELETE FROM EVENT", function (err, rows, fields) {
		if (err) 
			console.log("Something went wrong event data . "+err);
		else
			console.log("Cleared data from table");
	});
	
	connection.query("DELETE FROM ALARM", function (err, rows, fields) {
		if (err) 
			console.log("Something went wrong. "+err);
		else
			console.log("Cleared data from table");
	});
}

});

app.get('/StoreAllFiles', function(req, res) {
	if(connection){
		var uploadFolderName = './uploads/';
		var arrFileNames = new Array();
		let calArr = new Array();
		//let objCal = {};

		fs.readdir(uploadFolderName, (err, files) => 
		{
			if (err) {
				console.error(err);
			} else {
				console.log('Directory is:', files);
			}
		  files.forEach(file => {
			//console.log('file: ' + file);
			arrFileNames.push(file);
			
			let fileInfo = testA3.calOpen(uploadFolderName + file);
			let eventInfo = testA3.calOpenEvent(uploadFolderName + file);
			let eventOptInfo = testA3.calOpenOptionalEvent(uploadFolderName + file);
			//Extracting individual info from Calendar/File 
			let calData = JSON.parse(fileInfo);
			//Extracting individual info from Event
			let eventData = JSON.parse(eventInfo);
			//Extracting individual optional info from Event
			let eventOptData = JSON.parse(eventOptInfo);
			
			let JSONFile = JSON.stringify(eventData);
			
			if (calData){
				console.log("dataBase cal returned "+calData.version+"\n");
			}
			else{
				console.log("dataBase caldoesn't Exist\n");
			}
			console.log('!!: ' + JSONFile);

			let fileTable = [ 
				"INSERT INTO FILE (file_Name, version, prod_id) VALUES ('"+file+"','"+calData.version+"','"+calData.prodID+"')",
			];
			
			var checkIfFileExistsQuery = "SELECT * FROM FILE WHERE file_Name='"+file+"'";
			
			
			//
			connection.query(checkIfFileExistsQuery, function (err, rows, fields) {
				
				
				
				if(rows && rows.length > 0){
					console.log('file already exists, skip insert: ' + JSON.stringify(rows));
				}
				else{
					console.log('file does not exist in table yet');
					//1. Inserts the file into the database
					//for (let rec of fileTable){
					//connection.query(rec, function (err, rows, fields) {
					connection.query(fileTable[0], function (err, rows, fields) {
						if (err) console.log("Something went wrong file table. "+err);		
							
						var idInserted = rows.insertId;
						
						console.log('id inserted: ' + idInserted);
						
						//2. Once the FILE has been inserted, insert the EVENT data
						let eventTable = new Array();
						//eventData returns an array of obj, contains another obj inside that obj
						for(let x = 0; x < eventData.length; x++){
							let currentEvent = eventData[x];
							//console.log('eventOptData[x]: ' + eventOptData[x]);
							let currentOptEvent = eventOptData[x];
							if(currentEvent){
								let startTime = currentEvent.startDT;
								let date;
								let time;
								let location = '';
								let organizer = '';
								if(startTime){
									date = startTime.date;
									time = startTime.time;
								}
								let summary = currentEvent.summary;
								if(currentOptEvent){
									location = currentOptEvent.location;
									organizer = currentOptEvent.organizer;
								}
								
								console.log('location: ' + location);
								//creating the query with the foreign key reference
								eventTable.push( 
									"INSERT INTO EVENT (summary, start_time, location, organizer, cal_file) VALUES ('"+summary+"','"+date + time+"','"+location+"','"+organizer+"','"+idInserted+"')",
								);
							}
							
						}
						
						//Insert the data of eventTable to database
						let alarmTable = new Array();
						for(var x in eventTable){
							var eventQuery = eventTable[x];
							if(eventQuery){
								connection.query(eventQuery, function (err, rows, fields) {
									if (err) console.log("Something went wrong event table. "+err);	
									var idEvent = rows.insertId;
									alarmTable.push( 
										"INSERT INTO ALARM (action, `trigger`, event) VALUES ('Some_action','Some_trigger','"+idEvent+"')",
									);	
									
									for (var v in alarmTable){
										let alarmQuery = alarmTable[v];
										if (alarmQuery){
											connection.query(alarmQuery, function (err, rows, fields) {
												if (err) console.log("Something went wrong event table. "+err);	
												var alarmId = rows.insertId;
												//console.log('alarm was inserted: ' + alarmId);
											})
										}
									}
								})
							}
						}		
					})
				}
			});
			
		  });
		  
			res.send({ 
				result: true
		   });
		});
		
		
	}
});

app.get('/QueryOne', function(req, res) {


if(connection){
	connection.query("SELECT * FROM EVENT ORDER BY start_time", function (err, rows, fields) {  
		if (err) 
			console.log("Something went wrong. "+err);
		else {
			console.log("Database Event contents sorted by start_time:");
			for (let row of rows){
				console.log("summary: "+row.summary+" start_time: "+row.start_time, "location: "+row.location, "organizer: "+row.organizer, "cal_file: "+row.cal_file);
			}
			
			//return the rows to the front end
			res.send({ 
				dataRows: rows
		   });
		}
	});
}
});

app.get('/QueryTwo', function(req, res) {


if(connection){
	 console.log('QueryTwo data: ' + req.query.calData);
	 if(req.query.calData)
	  {
		  //if JSON.parse it will turn from string to obj
		  var file = req.query.calData;		  
		  
		  //var fileNameDir = './uploads/'+ file;
		  
		  console.log('Query TWo file: ' + file);
		  
		  
		  var joinQuery = "SELECT EVENT.summary as summary," +
					"EVENT.start_time as start_time, " +
					"FILE.file_name as file_name, " +
					"EVENT.cal_file as cal_file "  +
					"FROM EVENT LEFT JOIN FILE ON EVENT.cal_file = FILE.cal_id " +
					"WHERE FILE.file_name = '"+file+"'" ;
					
		  connection.query(joinQuery, function (err, rows, fields) { 
			   
				if (err) 
					console.log("Something went wrong with query two"+err);
				else {
					console.log('rows of Event Query: ' + JSON.stringify(rows));
				}
				res.send({ 
					dataRows: rows
				});
				
		  });
		  
      }	 
 }	 

 else{
	 console.log('no connection found for query two');
 }
	  
});

app.get('/QueryThree', function(req, res) {


if(connection){
	//var eventDateQuery = "SELECT * FROM EVENT ORDER BY start_time";
	//var eventDateQuery = "SELECT event_id, location, organizer, count(*) start_time, start_time FROM EVENT " + 
	//"GROUP BY start_time HAVING count(*) > 1;";
	
	var eventDateQuery = "SELECT event1.event_id, event1.organizer, event1.summary, event1.start_time, event2.start_time from EVENT AS event1, EVENT AS event2 " + 					
					"WHERE event1.start_time = event2.start_time " +
					"AND event1.event_id <> event2.event_id ";
 
	
	connection.query(eventDateQuery, function (err, rows, fields) {  
		if (err) 
			console.log("Something went wrong. "+err);
		else {
			//console.log("Database Event contents sorted by summary:");
			console.log('Third query result: ' + JSON.stringify(rows));
			for (let row of rows){
				console.log("event_id: "+row.event_id+" start_time: "+row.start_time);
			}
			
			//return the rows to the front end
			res.send({ 
				dataRows: rows
		   });
		}
	});
}
});

app.get('/dbStatus', function(req, res) {

    var countObj = {};
	var countFile = 0;
	var countEvent = 0;
	var countAlarm = 0;
	
	if(connection){
	//select from the file table, and loop through the results
		connection.query("SELECT count(*) as countFile FROM FILE", function (err, result) { 
			let fileString = JSON.stringify(result);
			let fileParse = JSON.parse(fileString);
			if (err) 
				console.log("Something went wrong. "+err);
			else {
				console.log("Number of records in FILE" + JSON.stringify(result));
				if(result.length > 0){
					countObj.countFile = result[0].countFile;
				}
			}
			
			//Get count of events
			connection.query("SELECT count(*) as countEvent FROM EVENT", function (err, result) {  
					if (err) 
						console.log("Something went wrong. "+err);
					else {
						console.log("Number of records in Event" + JSON.stringify(result));
						if(result.length > 0){
							countObj.countEvent = result[0].countEvent;
						}
						
					}
					//Get count of alarms
					connection.query("SELECT count(*) as countAlarm FROM ALARM", function (err, result) {  
					if (err) 
						console.log("Something went wrong. "+err);
					else {
						console.log("Number of records in ALARM" + JSON.stringify(result));
						if(result.length > 0){
							countObj.countAlarm = result[0].countAlarm;	
						}
						//return the rows to the front end
						res.send({ 
							countObj: countObj
					   });
						
					}
				});
			});
					
		});
	}
});

app.get('/QueryFour', function(req, res) {

	if(connection){
	//var eventDateQuery = "SELECT * FROM EVENT ORDER BY start_time";
	//var eventDateQuery = "SELECT event_id, location, organizer, count(*) start_time, start_time FROM EVENT " + 
	//"GROUP BY start_time HAVING count(*) > 1;";
	
	var FileSummaryQuery = "SELECT EVENT.summary as summary," +
					"FILE.file_name as file_name, " +
					"FILE.version as version, " +
					"EVENT.cal_file as cal_file "  +
					"FROM EVENT LEFT JOIN FILE ON EVENT.cal_file = FILE.cal_id ";
 
	
		connection.query(FileSummaryQuery, function (err, rows, fields) {  
			if (err) 
				console.log("Something went wrong. "+err);
			else {
				//console.log("Database Event contents sorted by summary:");
				console.log('Fourth query result: ' + JSON.stringify(rows));
				
				//return the rows to the front end
				res.send({ 
					dataRows: rows
			   });
			}
		});
	}
});

app.get('/QueryFive', function(req, res) {

	if(connection){
	//var eventDateQuery = "SELECT * FROM EVENT ORDER BY start_time";
	//var eventDateQuery = "SELECT event_id, location, organizer, count(*) start_time, start_time FROM EVENT " + 
	//"GROUP BY start_time HAVING count(*) > 1;";
	
	/*var FileIDQuery = "SELECT * FROM EVENT.start_time as start_time," +
					"WHERE EVENT.start_time = '1997'";*/
					
	var FileIDQuery = "SELECT FILE.file_name as file_name, " +
							"FILE.version as version, " +
							"EVENT.organizer as organizer "  +
							"FROM EVENT LEFT JOIN FILE ON EVENT.cal_file = FILE.cal_id " +
							"WHERE FILE.version = '3'" ;
	
	/*var FileIDQuery = "SELECT event1.event_id, event1.start_time, event2.start_time from EVENT AS event1, EVENT AS event2 " + 					
					"WHERE event1.start_time = '1997' " +
					"AND event1.event_id <> event2.event_id ";*/
	
		connection.query(FileIDQuery, function (err, rows, fields) {  
			if (err) 
				console.log("Something went wrong. "+err);
			else {
				//console.log("Database Event contents sorted by summary:");
				console.log('Fifth query result: ' + JSON.stringify(rows));
				
				//return the rows to the front end
				res.send({ 
					dataRows: rows
			   });
			}
		});
	}
});

app.get('/endConnection', function(req, res) {
	if(connection){
		connection.end();
	}
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
