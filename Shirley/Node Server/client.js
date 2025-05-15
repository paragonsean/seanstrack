http = require( "http" );
url  = require( 'url' );

/******************************************************
*                  get_response_handler               *
*******************************************************/
function get_response_handler( response )
{
console.log( "Response: " + res.statusCode );
console.log( "Headers: "  + response.headers );
console.log( "Body: "     + response.body );
}

/******************************************************
*                       error_handler                 *
*******************************************************/
function error_handler( e )
{
console.log( "Got error: " + e.message );
}

/******************************************************
*                     Main Program                    *
*******************************************************/
var request;

request = http.get( "http://localhost:8000", get_response_handler );
request.on( "error", error_handler );

