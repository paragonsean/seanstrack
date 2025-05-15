var http = require('http');
var fs   = require('fs');
var url  = require('url');
var ejs = require('ejs');

module.exports = function(file,type,lang,request,response){
	fs.readFile(file, 'utf-8',function(err, content){
        if ( err ) {
            /*
             ---------------------------
             Send a 404 Not Found status
             --------------------------- */
            response.writeHead(404, {"Content-Type": "text/plain; charset=UTF-8"});
            response.write(err.message);
            response.end();
        }
        else {
            response.writeHead( 200, {"Content-Type": type} );
            if(type == "text/html; charset=UTF-8") {
                var renderedHtml = ejs.render(content, {lang: lang});
                response.end(renderedHtml);
            }
            else{
                response.write(content);
                response.end();
            }
        }
	});
}

