// server for analog sensor setup
var http = require('http');
var fs   = require('fs');
var url  = require('url');

var asensorDir = 'C:\\V8Copy\\DATA\\asensor_new.csv';

/*************************************************************
*                       request_handler                      *
**************************************************************/
var request_handler = function(request, response){

	var pathname = url.parse( request.url ).pathname;

	if(pathname === '/getsensor'){
		fs.readFile(asensorDir, function(err, content){
			if (err) throw err;
			response.write(content.toString());
			response.end();
		});
	}
	else if(pathname == "/analog_sensor_setup.htm"){   
		var type = "text/html; charset=UTF-8"; 
		fs.readFile("analog_sensor_setup.htm", function(err, content) {
			if ( err ) 
				{
				/*
				---------------------------
				Send a 404 Not Found status
				--------------------------- */
				response.writeHead( 404, {"Content-Type": "text/plain; charset=UTF-8"} );
				response.write( err.message );
				response.end();
				}
			else 
				{
				response.writeHead( 200, {"Content-Type": type} );
				response.write( content );
				response.end();   
				}
		});
		
	}
	
	else if(pathname == "/postsensor"){
			var string = decodeURIComponent(url.parse(request.url).query); 
			fs.writeFile(asensorDir,string,function(err){
				if (err) throw err;
				console.log("changes saved!");
				response.end();
				
			});
		}
		
	else if(pathname == "/js/jquery-2.1.4.min.js"){
		fs.readFile("js/jquery-2.1.4.min.js", function(err, content){
			if (err) {
                response.writeHead(404, {"Content-type":"text/plain"});
                response.end("No Javascript Page Found.");
            } else{
                response.writeHead(200, {'Content-Type': 'text/javascript'});
                response.write(content);
                response.end(); 
            }
		});
	}
	else if(pathname == "/js/flotr2.min.js"){
		fs.readFile("js/flotr2.min.js", function(err, content){
			if (err) {
                response.writeHead(404, {"Content-type":"text/plain"});
                response.end("No Javascript Page Found.");
            } else{
                response.writeHead(200, {'Content-Type': 'text/javascript'});
                response.write(content);
                response.end(); 
            }
		});
	}



}

/*************************************************************
*                         Main Program                       *
**************************************************************/
var server = new http.Server();
server.listen( 8000 ); 

/*
-------------------------------------------------------------------
Node uses the "on()" method to register event handlers.
When the server gets a new request, run this function to handle it.
------------------------------------------------------------------- */
server.on( "request", request_handler );

