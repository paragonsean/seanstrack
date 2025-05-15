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

/*************************************************************
*                       request_handler                      *
**************************************************************/
function request_handler( request, response )
{
/*
-----------------------
Parse the requested URL
----------------------- */
var uuu = url.parse( request.url );
//console.log( request.url );
//console.log(uuu.pathname.substring(1));

/*
--------------------------------------------------------------------
Get local filename and guess its content type based on its extension
-------------------------------------------------------------------- */
filename = uuu.pathname.substring(1); // strip leading /

//console.log(filename);

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
/*
-----------------------------------------------------------------
Read the file asynchronously and pass the content as a single
chunk to the callback function. For really large files, using the
streaming API with fs.createReadStream() would be better.
----------------------------------------------------------------- */	
fs.readFile( filename, function(err, content) {
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

