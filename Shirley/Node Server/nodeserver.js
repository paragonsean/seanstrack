/****************************************************************************
* This is a simple NodeJS HTTP server that can serve files from the current *
* directory. From Javascript: The Definitive Guide, Example 12-2            *
* Connect to the server at http://localhost:8000 or http://127.0.0.1:8000   *
*****************************************************************************/
var http = require('http');
var fs   = require('fs');
var url  = require('url');

var filename;
var type;  
var myresponse;

/*************************************************************
*                         read_handler                       *
**************************************************************/
function read_handler( err, content )
{
if ( err ) 
    {
    /*
    ---------------------------
    Send a 404 Not Found status
    --------------------------- */
    myresponse.writeHead( 404, {"Content-Type": "text/plain; charset=UTF-8"} );
    myresponse.write( err.message );
    myresponse.end();
    }
else 
    {
    myresponse.writeHead( 200, {"Content-Type": type} );
    myresponse.write( content );
    myresponse.end();
    }
}

/*************************************************************
*                       request_handler                      *
**************************************************************/
function request_handler( request, response )
{
var u = url.parse( request.url );
console.log( request.url );

/*
--------------------------------------------------------------------
Get local filename and guess its content type based on its extension
-------------------------------------------------------------------- */
filename = u.pathname.substring(1); // strip leading /

switch( filename.substring(filename.lastIndexOf(".")+1) )  
    { 
    case "html":
    case "htm":      type = "text/html; charset=UTF-8"; break;
    case "js":       type = "application/javascript; charset=UTF-8"; break;
    case "css":      type = "text/css; charset=UTF-8"; break;
    case "csv":
    case "txt" :     type = "text/plain; charset=UTF-8"; break;
    case "manifest": type = "text/cache-manifest; charset=UTF-8"; break;
    default:         type = "application/octet-stream"; break;
    }

myresponse = response;
fs.readFile( filename, read_handler );
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

