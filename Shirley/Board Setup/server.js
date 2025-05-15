var fs = require('fs');
var path = require('path');
var ejs = require('ejs');
var express = require('express');
var bodyParser = require("body-parser");


/*******************************************************************
*                      getDirectories                              *
********************************************************************/
function getDirectories(srcpath) {
    return fs.readdirSync(srcpath).filter(function (file) {
        return fs.statSync(path.join(srcpath, file)).isDirectory();
    });
}

/*******************************************************************
*                        getBoardNumber                                  *
********************************************************************/
function getBoardNumber(filename) {
    var text = fs.readFileSync(filename).toString();
    var lines = text.split('\n');
    var secondLine = lines[1];
    var secondLineCont = secondLine.split(',');
    return parseInt(secondLineCont[1]);
}



/*******************************************************************
*        Output Machine list and Board Number                      *
********************************************************************/
var machineDir = 'C:\\V8Copy\\DATA';
// machine name array
var subdir = getDirectories(machineDir);
// list with machine name and board number
var machineBoardNumber = {};
for (var j = 0; j < subdir.length; j++) {
    machineName = subdir[j];
    file = machineDir + '\\' + machineName + '\\machine_setup.csv';
    machineBoardNumber[machineName] = getBoardNumber(file);
}

/*******************************************************************
*                        getBoardInfo                             *
********************************************************************/
//read the board info and convert into an array format so javascript can parse it
// if in future we read JSON format string, this needs to be changed accordingly
var boards = "[[" + fs.readFileSync(machineDir + '\\Ftii_boards.txt').toString().replace(/[;]+/g,',').replace(/[\n]+/g,'],[')+']]';

/*******************************************************************
*                        Create Server                             *
********************************************************************/
var content = fs.readFileSync('boardSetUp.htm','utf-8');
var compiled = ejs.compile(content);

var app = express();
app.engine('.html', require('ejs').__express);
app.set('view engine', 'html');

app.use(function(req, res, next) {
  res.header("Access-Control-Allow-Origin", "*");
  res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS, PUT, PATCH, DELETE');
  next();
});


app.get('/',function(req,res){
	res.end(compiled({machineName: subdir, machineBoardNumber: machineBoardNumber, boards: boards}));
});


//get data from client
app.use(bodyParser.urlencoded({extended:false}));
app.post('/',function(req,res){
	// boardTable and machineTable are strings, need to JSON.parse first
	var boardTable = JSON.parse(req.body.boardTable);
	var machineTable = JSON.parse(req.body.machineTable);
	//console.log(machineTable);
	// execute the functions here
	writeBoardChanges(boardTable);
	writeMachineChanges(machineTable);
	console.log('changes saved!');
	res.end("changes saved!");
});
app.listen(3000);


/*******************************************************************
*                        Write board changes                       *
********************************************************************/
// write the array directly to the file, so it can be read in array format next time
// write to a different file first
function writeBoardChanges(data){
	var boardFile = machineDir + '\\Ftii_boards2.txt'; // write to a new file first
	fs.writeFileSync(boardFile, JSON.stringify(data));	// stringify the array, so next time need to parse first
}

/*******************************************************************
*                        Write machine changes                      *
********************************************************************/
// open the file, read the whole content, replace the second line, write to the file
function writeMachineChanges(data){
	for (var j=0;j<data.length;j++){
		var machine = data[j]['machine name'];
		var machineFileOld = machineDir + '\\' + machine + '\\machine_setup.csv';
		var text = fs.readFileSync(machineFileOld).toString();
		var newboardnumber = data[j]['board number'];
		//use RegExp to replace the board number
		var newtext = text.replace(/Fast Trak Board Number,\d/g,'Fast Trak Board Number,' + newboardnumber);
		var machineFileNew = machineDir + '\\' + machine + '\\machine_setup2.csv';
		fs.writeFileSync(machineFileNew, newtext);
	}
}