var fs = require('fs'),
    path = require('path');

	
/*******************************************************************
*                      getDirectories                              *
********************************************************************/
function getDirectories(srcpath) {
  return fs.readdirSync(srcpath).filter(function(file) {
    return fs.statSync(path.join(srcpath, file)).isDirectory();
  });
}

/*******************************************************************
*                        getBoardNumber                                  *
********************************************************************/
function getBoardNumber(filename){
  var text = fs.readFileSync(filename).toString();
 
  var lines = text.split('\n');
  var secondLine = lines[1];
  var secondLineCont = secondLine.split(',');
  return parseInt(secondLineCont[1]);
  }
  

/*******************************************************************
*        Output Machine list and Board Number                      *
********************************************************************/
var machineDir = 'C:\\V8\\DATA';
// machine name array
var subdir = getDirectories(machineDir);
// list with machine name and board number
var machineBoardNumber = {};
for (var j=0; j<subdir.length; j++){
	machineName = subdir[j];
	file = machineDir + '\\' + machineName + '\\machine_setup.csv';
	machineBoardNumber[machineName] = getBoardNumber(file);
}



