// server for analog sensor setup
var http = require('http');
var url  = require('url');
var querystring = require("querystring");
var fs = require("fs");

var getData = require("./routes/getData");
var postData = require("./routes/postData");
var getDirectories = require("./routes/getDirectories");

var myDir = require("./routes/myDir");
var createMachine = require("./routes/createMachine");
var rimraf = require("./routes/rimraf");

/*************************************************************
*                       request_handler                      *
**************************************************************/
var request_handler = function(request, response){
	
	//CORS allow ANY origin to access
	response.setHeader("Access-Control-Allow-Origin", "*");

	var path = url.parse( request.url ).pathname;
    var type;

    var ext=path.substring((path.lastIndexOf("."))+1);

    if(ext == "html" || ext == "htm") type = "text/html; charset=UTF-8";
    else if(ext == "js") type = "text/javascript; charset=UTF-8";
    else if(ext == "css") type = "text/css; charset=UTF-8";
    else if(ext == "csv" || ext == "txt") type = "text/css; charset=UTF-8";
    else if(ext == "manifest") type = "text/cache-manifest; charset=UTF-8";
    else type = "application/octet-stream";

    if(path == "/") {
        getData("views/index.htm", "text/html; charset=UTF-8", "English", request, response);
    }

    else if(ext == "png"){
        var fileStream = fs.createReadStream(decodeURIComponent(path.substring(1)));
        response.writeHead(200,{'Content-Type': 'image/png'});
        fileStream.on('open',function(){
            fileStream.pipe(response);
        });
        fileStream.on('error', function(err) {
            response.end(err);
        });
    }

    else if(path == "/getDirectories"){
        var sourceFolder = decodeURIComponent(url.parse(request.url).query);
        getDirectories(sourceFolder, request, response);
    }

    else if(path =="/createMachine"){
        var arr = querystring.parse(url.parse(request.url).query);
        var sourceFolder = decodeURIComponent(arr.srcDir);
        var destFolder = decodeURIComponent(arr.dstDir);
        createMachine(sourceFolder,destFolder,request,response);

    }

    else if(path =="/copyFiles"){
        var arr = querystring.parse(url.parse(request.url).query);
        var sourceFolder = decodeURIComponent(arr.srcDir);
        var destFolder = decodeURIComponent(arr.dstDir);
        myDir.myCopyFiles(sourceFolder,destFolder,{},function(){
            response.end();
        });

    }

    else if(path == "/deleteFolder"){
        var folder = decodeURIComponent(url.parse(request.url).query);
        rimraf(folder,function(){
            response.end();
        });
    }

    else if(path == "/makeDir"){
        var dirPath = decodeURIComponent(url.parse(request.url).query);
        fs.mkdir(dirPath,function(){
            response.end();
        });
    }

	else if(request.method == "GET"){
        var lang = "English";
        if(url.parse(request.url).query){
            lang = url.parse(request.url).query;
        }
        getData(decodeURIComponent(path.substring(1)),type,lang,request,response);
	}
    else if(request.method == "POST"){
        postData(decodeURIComponent(path.substring(1)),request,response);
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

