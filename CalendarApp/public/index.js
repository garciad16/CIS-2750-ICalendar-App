//index.js is the browser javascript of the client side, and interacts with the server side by calling the
//endpoints using AJAX when HTML page is loaded
//The url is the name of the endpoint from the app.js file
//if endpoint is successful it will go into the successful function
//you populate HTML within the success function using the data argument or parameter 
//the data arguement/parameter comes from the value that you send into the app.js endpoint

// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    // On page-load AJAX Example    
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/getallfilenames',   //The server endpoint we are connecting to
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string, 
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
            
            //convert to array
            //loop through file names
            //call function to generate table and pass fileName
           
            console.log('get all files: ' + data.fileNames); 
            
            if(data.fileNames.length == 0){
				$('#tableBody1').append('No Files');
				$('#myDropdown').append('<a></a>' + '<a></a>' + '<a></a>');
			}
            
            var arr = data.fileNames;
            var url = "/uploads/";

		var string = JSON.stringify(data);
		var calObj = JSON.parse(string);

		var calArr = calObj.calNames;
	console.log(JSON.stringify(calObj));  
	
	//add an option field instead of a url
	var select = $("<option>");
	
	
	select.attr({
		type: "option",
	});
	
	select.attr({
		type: "option",
	});
	//option.value(arr[o]);
	select.text('--Select File--');
	
	//select.on("onload", getFileInfoDropdown2);
	
	$('#eventDropdown').append(
		select
	);          

	for (var o in calArr){
		$('#tableBody1').append('<tr>'
			+ '<td><a href='+ url+arr[o]+'>' + arr[o]+ '</td>'
			+ '<td>'+calArr[o].version+ '</td>'
			+ '<td>'+calArr[o].prodID +'</td>'
			+ '<td>'+calArr[o].numEvents+ '</td>'
			+ '<td>'+calArr[o].numProps+ '</td>'
		);

		//add an option field instead of a url
		var option = $("<option>");
		var option2 = $("<option>");
		
		option.attr({
			type: "option",
		});
		
		option2.attr({
			type: "option",
		});
		//option.value(arr[o]);
		option.text(arr[o]);
		option.on("click", getFileInfoDropdown);
		
		option2.text(arr[o]);
		option2.on("click", getFileInfoDropdown2);
		
		//get fileInfo url
		$('#myDropdown').append(
			option		
		);
		
		$('#eventDropdown').append(
			option2
		);
		
		/*$( "#dropdownitem" ).on( "click", function() {
				alert( $( this ).text() );
			});*/
		
	}

	},
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error); 
        }
    });
    
});

//gets called when the user selects the file name from the dropdown
function getFileInfoDropdown(){
		var currentField = this;
		var val = this.value; //filename
		dropdownAdd(val);
		showAlarms(val);
}

function getFileInfoDropdown2(){
		var currentField = this;
		var val = this.value; //filename
		dropdownEventAdd(val);
}

//File Log Panel, add the file selected to the table Panel
function addRow1() {

	//get the value of the file input
	//populate the html table
	var longName = $('.uploadFile').val(); 
	//this returns the file name from the file input
	//but it contains a fake path, so we parse out the file name only
	var shortName = "";
	//adding an if statement so that we only add the row if there is a file name
	if(longName){
		var fileArr = longName.split("\\");
		if(fileArr && fileArr.length > 0){
			shortName = fileArr[fileArr.length - 1];
		}
		else{
			shortName = longName;
		}
		var ext = shortName.split('.');
		
		//console.log("The shortname filename is:" + ext);
		
		$.ajax({
        		type: 'get',
        		dataType: 'json',
        		url: '/getallfilenames2?name='+shortName,
        		success: function(data){
			console.log('successfully called filenames2');

			var string = JSON.stringify(data);
			var objCal = JSON.parse(string);

			var cal = objCal.calNames;

			console.log('shortName: ' + shortName);
	                var rowNum = $('#tableBody1 > tr').length + 1;
        	        var url = "/uploads/";
                	if (ext[1] === 'ics'){
                        	$('.statusPanel').append('File added is of .ics form, return OK<br>');
                        	$('#tableBody1').append('<tr>'
                                	+ '<td><a href='+url+shortName+'>' + shortName + '</td>'
                                	+ '<td>'+cal.version +'</td>'
                                	+ '<td>'+cal.prodID +'</td>'
                                	+ '<td>'+cal.numEvents +'</td>'
                                	+ '<td>'+cal.numProps + '</td>'
                        	);
                	}
                	else{
                        	var noFiles = 'Invalid File';
                        	$('.statusPanel').append('File added is not of.ics form, return INV_FILE<br>');
                        	$('#tableBody1').append('<tr>'
                                	+ '<td></td>'
                                	+ '<td></td>'
                                	+ '<td>'+ noFiles +'</td>'
                                	+ '<td></td>'
                                	+ '<td></td>'
                        	);
                        	return;
                	}

			}
		});


	}
	
}

function dropdownAdd(fileName) {
 
	$.ajax({
		type: 'get',
		dataType: 'json',
		url: '/getallfilenames3?name='+fileName,
		success: function(data){
			console.log('successfully called filenames3');
			
			var string = JSON.stringify(data);
			var objEvt = JSON.parse(string);

			var evt = objEvt.evtNames;
			
			var string2 = JSON.stringify(data);
			var objEvt2 = JSON.parse(string2);

			var dt = objEvt2.dtNames;
			
			console.log('getting array evt:' +string);
			
			                       	
			for (var o in evt){
				var rowNum = $('#tableBody2 > tr').length + 1;  
				$('#tableBody2').append('<tr>'
					+ '<td>' + rowNum + '</td>'
					+ '<td>'+dt[o].date+ '</td>'
					+ '<td>'+dt[o].time+'</td>'
					+ '<td>'+evt[o].summary+ '</td>'
					+ '<td>'+evt[o].numProps+ '</td>'
					+ '<td>'+evt[o].numAlarms+'</td>'
					+ '<td><button2 onclick="showAlarms()">Double click me</button2></td>'
					+ '<td><button3 onclick="showOtherProps()">Double click me</button3></td>'
				);    
			}
		}
	});
 
}

function clearPanel(){
	$('.statusPanel').empty()
}

function showAlarms(fileName){
	$( "button2" ).click(function() {
		$( this ).replaceWith("Trigger: NULL\n Action: NULL\n numProps: 0\n");
	});
}

function showOtherProps(){
	$( "button3" ).click(function() {
		$( this ).replaceWith("Attach: NULL\n Duration: NULL\n Repeat: NULL\n");
	});
}

$.fn.serializeObject = function(){
    var o = {};
    var a = this.serializeArray();
    $.each(a, function() {
        if (o[this.name] !== undefined) {
            if (!o[this.name].push) {
                o[this.name] = [o[this.name]];
            }
            o[this.name].push(this.value || '');
        } else {
            o[this.name] = this.value || '';
        }
    });
    return o;
};

$(function() {
    $('#form1').submit(function() {
		 var fileJSON = {};
		 var calJSON = {};
		 var uidJSON = {};
		 var dtJSON = {};
		 var wrapperObj = {};
		 var emptyObj = {};
		 
		 
		 var fileName = $('#newFileName').val();
		 
		 fileJSON.fileName = fileName;
		 //alert(JSON.stringify(fileJSON));
		 
		 var version = $('#version').val();
		 var prodID = $('#prodID').val();
		 
		 calJSON.version = version;
		 calJSON.prodID = prodID;		 
		 //alert(JSON.stringify(calJSON));
		 
		 var UID = $('#UID').val();
		 
		 uidJSON.UID = UID;		 
		 //alert(JSON.stringify(uidJSON));
		 
		 var startDate = $('#startDT_date').val();
		 var startTime = $('#startDT_time').val();
		 var creationDate = $('#creationDT_date').val();
		 var creationTime = $('#creationDT_time').val();
		 
		 dtJSON.startDate = startDate;
		 dtJSON.startTime = startTime;
		 dtJSON.creationDate = creationDate;
		 dtJSON.creationTime = creationTime;		 
		 //alert(JSON.stringify(dtJSON));
		 
		 if (version === "" || prodID === ""){
			console.log('version or prodID has an empty string');
			wrapperObj.calJSON = emptyObj;
		 }
		 else {
			console.log('version has string');
			wrapperObj.calJSON = calJSON;
		 }
		 
		 if (UID === ""){
			console.log('UID has an empty string');
			wrapperObj.uidJSON = emptyObj;
		 }
		 else {
			console.log('UID has string');
			wrapperObj.uidJSON = uidJSON;
		 }
		 
		 if (startDate === "" || startTime === "" || creationDate === "" || creationTime === ""){
			console.log('DT has an empty string');
			wrapperObj.dtJSON = emptyObj;
		 }
		 else {
			console.log('DT has string');
			wrapperObj.dtJSON = dtJSON;
		 }
		 
		 wrapperObj.file = fileName;
         //$('#result1').text(JSON.stringify($('#form1').serializeObject()));
        
        $.ajax({
			type: 'get',
			dataType: 'json',
			//data: wrapperObj,
			url: '/newCalendar?calData='+JSON.stringify(wrapperObj),
			success: function(data){
				console.log("newEvent func returns:" + data.result);
				
				let calStatus = data.result;
				
				if (calStatus === 'NULL'){
					$('.statusPanel').append('Invalid DT for new Cal/Not a valid iCal file. Need atleast one event<br>');	
				}				
			}
		});
        return false;
    });
});

function dropdownEventAdd(fileName) {
    $('#form2').submit(function() {
        var uidJSON = {};
		var dtJSON = {};
		var wrapperObj = {};
		var emptyObj = {};
		//alert('eventDropdown value: ' + $('#eventDropdown'
		var UID = $('#newUID').val();
		 
		 uidJSON.UID = UID;		 
		 //alert(JSON.stringify(uidJSON));
		 
		 var startDate = $('#startEVT_date').val();
		 var startTime = $('#startEVT_time').val();
		 var creationDate = $('#creationEVT_date').val();
		 var creationTime = $('#creationEVT_time').val();
		 
		 dtJSON.startDate = startDate;
		 dtJSON.startTime = startTime;
		 dtJSON.creationDate = creationDate;
		 dtJSON.creationTime = creationTime;
		 
		 if (UID === ""){
			console.log('UID has an empty string');
			wrapperObj.uidJSON = emptyObj;
		 }
		 else {
			console.log('UID has string');
			wrapperObj.uidJSON = uidJSON;
		 }
		 
		 if (startDate === "" || startTime === "" || creationDate === "" || creationTime === ""){
			console.log('DT has an empty string');
			wrapperObj.dtJSON = emptyObj;
		 }
		 else {
			console.log('DT has string');
			wrapperObj.dtJSON = dtJSON;
		 }
		 
		 wrapperObj.file = fileName;
         //$('#result1').text(JSON.stringify($('#form1').serializeObject()));
        
         console.log('JSON.stringify(wrapperObj) +' +  JSON.stringify(wrapperObj));
        $.ajax({
			type: 'get',
			dataType: 'json',
			//data: wrapperObj,
			url: '/newEvent?calData='+JSON.stringify(wrapperObj),
			success: function(data){
				//fileReturn string is return here by data obj from app.js
				console.log('successfully called wrapperObj');
				console.log("newEvent func returns:" + data.result);
				
				let eventStatus = data.result;
				
				if (eventStatus === 'NULL'){
					$('.statusPanel').append('Invalid Event Date/Not a valid iCal file<br>');	
				}				
			}
		});
        return false;
    });
}

$( "#uploadForm" ).submit(function( event ) {
  var fileName = $('.uploadFile').val();
  var fileArr = fileName.split("\\");
  var shortName = "";
  shortName = fileArr[fileArr.length - 1];
  var ext = shortName.split('.');
  alert(shortName);
  
  if(ext[1] != 'ics'){ //validation here
	//update html here
	//alert('false');
	 $('.statusPanel').append('File added is not of.ics form, return INV_FILE<br>');
	 return false;
  }
  else{
	  
	  $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/getFiles',   //The server endpoint we are connecting to
        success: function (data) {
			var arr = data.fileNames;
		
			for (var o in arr){
				if (shortName === arr[o]){
					alert("if statement works");
						$('.statusPanel').append('File already exists in server<br>');
						return false;
				}
			}
			
			addRow1();
			$('.statusPanel').append('File added is of.ics form, return OK<br>');
			return true;
			
		},
		fail: function(error) {
            // Non-200 return, do something with error
            //console.log('fail to get files');
        }
		
	 });	
	 
	 return false;
  }
  
  
  //event.preventDefault();
 
});

$( "#dblogin" ).submit(function( event ) {
	
	var loginObj = {};
	var userName = $("#username").val();
	var password = $("#password").val();
	var database = $("#database").val();
	
	loginObj.userName = userName;
	loginObj.password = password;
	loginObj.database = database;
	
	$.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/login?data='+JSON.stringify(loginObj),   //The server endpoint we are connecting to
        success: function (data) { 

			if(data.connected === true){
				alert("You are connected");
				
				
				//get the number of rows from db
				
				$.ajax({
					type: 'get',            //Request type
					dataType: 'json',       //Data type - we will use JSON for almost everything 
					url: '/dbStatus',   //The server endpoint we are connecting to
					success: function (data) {
						console.log('Able to retrieve DB status');
						let tableObj = data.countObj;
						
						let fileNum = tableObj.countFile;
						console.log("fileNum is : " + fileNum);
						
						let eventNum = tableObj.countEvent;
						console.log("eventNum is : " + eventNum);
						
						let alarmNum = tableObj.countAlarm;
						console.log("alarmNum is : " + alarmNum);
						
						$('.statusPanel').append('Database has '+ fileNum +' files, '+ eventNum +'events, and ' +alarmNum + ' alarms<br>');
					},
					fail: function(error) {
						// Non-200 return, do something with error
						//console.log('fail to get files');
					}
					
				 });	
				
			}
			else {
				alert("Unable to connect, enter your login again");
			}
		},
		fail: function(error) {
			
			console.log('Login was a failure!!');	
				//if failure, alert message to user to re-enter
				alert("Incorrect information. Enter username, password and Database name again");
		}
	});
	
	event.preventDefault();
});

//var modal = document.getElementById('id01');

// When the user clicks anywhere outside of the modal, close it
window.onclick = function(event) {
  if (event.target == modal) {
    modal.style.display = "none";
  }
}

$(document).ready(function() {
	$.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/getOptionalEvents',   //The server endpoint we are connecting to
        success: function (data) {
			console.log('Able to extract optional properties');
		},
		fail: function(error) {
            // Non-200 return, do something with error
            //console.log('fail to get files');
        }
		
	 });	
});

function clearDB(){
	$.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/ClearData',   //The server endpoint we are connecting to
        success: function (data) {
			console.log('Able to extract optional properties');
		},
		fail: function(error) {
            // Non-200 return, do something with error
            //console.log('fail to get files');
        }
		
	 });	
	 
	 $.ajax({
			type: 'get',            //Request type
			dataType: 'json',       //Data type - we will use JSON for almost everything 
			url: '/dbStatus',   //The server endpoint we are connecting to
			success: function (data) {
				console.log('Able to retrieve DB status');
				let tableObj = data.countObj;
				
				let fileNum = tableObj.countFile;
				console.log("fileNum is : " + fileNum);
				
				let eventNum = tableObj.countEvent;
				console.log("eventNum is : " + eventNum);
				
				let alarmNum = tableObj.countAlarm;
				console.log("alarmNum is : " + alarmNum);
				
				$('.statusPanel').append('Database has '+ fileNum +' files, '+ eventNum +'events, and ' +alarmNum + ' alarms<br>');
			},
			fail: function(error) {
				// Non-200 return, do something with error
				//console.log('fail to get files');
			}
				
	 });
}

function QueryOneBtn(){
	$.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/QueryOne',   //The server endpoint we are connecting to
        success: function (data) {
			if(data){
				
				let queryData1 = data.dataRows;
				console.log('Able to extract query 1: ' + queryData1.length);
				for (let i = 0; i < queryData1.length; i++){
					let currentData = queryData1[i]; 
					console.log("queryData given" + currentData);
					$('#QueryTable1').append('<tr>'
                       + '<td>'+currentData.summary+'</td>'
                       + '<td>'+currentData.start_time +'</td>'              
                       + '<td>'+currentData.location +'</td>'
                       + '<td>'+currentData.organizer + '</td>'
                       + '<td>'+currentData.cal_file + '</td>'
                    );
				}
			}
			else {
				console.log("No Data or no Connection found");
			}
		},
		fail: function(error) {
            // Non-200 return, do something with error
            //console.log('fail to get files');
        }
		
	 });	
}

function storeFiles(){
	$.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/StoreAllFiles',   //The server endpoint we are connecting to
        success: function (data) {
			console.log('Able to Store files into DataBase');
			
			$.ajax({
				type: 'get',            //Request type
				dataType: 'json',       //Data type - we will use JSON for almost everything 
				url: '/dbStatus',   //The server endpoint we are connecting to
				success: function (data) {
					console.log('Able to retrieve DB status');
					let tableObj = data.countObj;
					
					let fileNum = tableObj.countFile;
					console.log("fileNum is : " + fileNum);
					
					let eventNum = tableObj.countEvent;
					console.log("eventNum is : " + eventNum);
					
					let alarmNum = tableObj.countAlarm;
					console.log("alarmNum is : " + alarmNum);
					
					$('.statusPanel').append('Database has '+ fileNum +' files, '+ eventNum +'events, and ' +alarmNum + ' alarms<br>');
				},
				fail: function(error) {
					// Non-200 return, do something with error
					//console.log('fail to get files');
				}
					
			});	
		},
		fail: function(error) {
            // Non-200 return, do something with error
            //console.log('fail to get files');
        }
     });
        
	
}

function QueryTwoBtn(){
		 	 	 
		 var fileName = $('#fileBox').val();

         //$('#result1').text(JSON.stringify($('#form1').serializeObject()));
        
        $.ajax({
			type: 'get',
			dataType: 'json',
			//data: wrapperObj,
			url: '/QueryTwo?calData='+fileName,
			success: function(data){
				if(data){
				
				let queryData2 = data.dataRows;
				console.log('Able to extract query 2: ' + queryData2.length);
				for (let i = 0; i < queryData2.length; i++){
					let currentData = queryData2[i]; 
					console.log("queryData given" + JSON.stringify(currentData));
					$('#QueryTable2').append('<tr>'
						+ '<td>'+currentData.file_name+'</td>'
						+ '<td>'+currentData.summary+'</td>'
						+ '<td>'+currentData.start_time +'</td>'             
						+ '<td>'+currentData.cal_file + '</td>'
                    );
				}
			}
							
		}
	});
	return false;
}

function QueryThreeBtn(){
	$.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/QueryThree',   //The server endpoint we are connecting to
        success: function (data) {
			if(data){
				
				let queryData3 = data.dataRows;
				console.log('Able to extract query 1: ' + queryData3.length);
				for (let i = 0; i < queryData3.length; i++){
					let currentData = queryData3[i]; 
					console.log("queryData given" + currentData);
					$('#QueryTable3').append('<tr>'
                       + '<td>'+currentData.summary+'</td>'
                       + '<td>'+currentData.start_time +'</td>'                               
                       + '<td>'+currentData.organizer + '</td>'
                       + '<td>'+currentData.event_id + '</td>'
                    );
				}
			}
			else {
				console.log("No Data or no Connection found");
			}
		},
		fail: function(error) {
            // Non-200 return, do something with error
            //console.log('fail to get files');
        }
	 });	
}

function QueryFourBtn(){
	$.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/QueryFour',   //The server endpoint we are connecting to
        success: function (data) {
			console.log("Query Four works");
			if(data){
				
				let queryData4 = data.dataRows;
				console.log('Able to extract query 4: ' + queryData4.length);
				for (let i = 0; i < queryData4.length; i++){
					let currentData = queryData4[i]; 
					console.log("queryData given" + JSON.stringify(currentData));
					$('#QueryTable4').append('<tr>'
                       + '<td>'+currentData.file_name+'</td>'
                       + '<td>'+currentData.version+'</td>'
                       + '<td>'+currentData.summary +'</td>'             
                       + '<td>'+currentData.cal_file + '</td>'
                    );
				}
			}
		},
		fail: function(error) {
            // Non-200 return, do something with error
            //console.log('fail to get files');
        }
	 });	
}

function QueryFiveBtn(){
	$.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/QueryFive',   //The server endpoint we are connecting to
        success: function (data) {
			console.log("Query Five works");
			if(data){
				
				let queryData5 = data.dataRows;
				console.log('Able to extract query 5: ' + queryData5.length);
				for (let i = 0; i < queryData5.length; i++){
					let currentData = queryData5[i]; 
					console.log("queryData given" + JSON.stringify(currentData));
					$('#QueryTable5').append('<tr>'
                       + '<td>'+currentData.file_name+'</td>'
                       + '<td>'+currentData.version+'</td>'
                       + '<td>'+currentData.organizer +'</td>'             
                    );
				}
			}
			
		},
		fail: function(error) {
            // Non-200 return, do something with error
            //console.log('fail to get files');
        }
	 });	
}

function endConnect(){
	alert("You have logged out of Database");
	$.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/endConnection',   //The server endpoint we are connecting to
        success: function (data) {
			console.log("End Connection works");
			
		},
		fail: function(error) {
            // Non-200 return, do something with error
            //console.log('fail to get files');
        }
	 });	
}

