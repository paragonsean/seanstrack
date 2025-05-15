var http = require("http");

function request_handler( request, response )
{
response.writeHead( 200, {"Content-Type": "text/plain"} );
response.write( "Hello World" );
response.end();
}

var myserver = http.createServer( request_handler );
myserver.listen( 8888 );
