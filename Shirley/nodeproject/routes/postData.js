var http = require('http');
var fs   = require('fs');
var url  = require('url');

module.exports = function(file, request, response){
	var string = decodeURIComponent(url.parse(request.url).query); 
		fs.writeFile(file,string,function(err){
			if (err) throw err;
			response.end();				
		});
}