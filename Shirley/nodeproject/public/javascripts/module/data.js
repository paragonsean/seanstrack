//read file and convert to array, write data to string

/*******************************************************************
 *                         readWAtt                              *
 ********************************************************************/
//read file with attribute names
//attribute name, attribute value\n....
//only one object
//e.g. machine_setup.csv
//output is a object with attName: name array, attValue: value array
function readWAtt(string) {
    var lines = string.split('\n');

    //check if the last line is empty, if so, remove it
    if(lines[lines.length-1] == "") {
        lines = lines.slice(0,-1);
    }

    var attName = [];
    var attValue = [];
    for (var j = 0; j < lines.length; j++) {
        var split = lines[j].split(",");
        attName[j] = split[0];
        attValue[j] = split[1];
    }
    return {
        attName: attName,
        attValue: attValue
    };
}

/*******************************************************************
 *                         writeWAtt                              *
 ********************************************************************/
//return a string of attName,attValue\n...
//read an array of attName and an array of attValue
function writeWAtt(attName, attValue){
    var result = "";
    for(var i = 0; i < attName.length; i++){
        result += attName[i] + "," + attValue[i] + "\n";
    }
    return result.slice(0,-1);
}

/*******************************************************************
 *                         readWOAtt                              *
 ********************************************************************/
//read files with no attribute names, only values
//format like obj1value1,obj1value2,obj1value3\nobj2value1,obj2value2,obj3value3
//e.g.Ftii_boards_new.txt
//output is an array[[[],[],[]],[[],[],[]],...]
function readWOAtt(text){
    var result = [];
    var lines = text.split("\n");

    //check if the last line is empty, if so, remove it
    if(lines[lines.length-1] == "") {
        lines = lines.slice(0,-1);
    }

    for(var i = 0; i < lines.length; i++){
        result[i] = lines[i].split(",");
    }
    return result;
}

/*******************************************************************
 *                         readWOAttJSON                              *
 ********************************************************************/
function readWOAttJSON(text){
    var lines = text.split("\r\n");

    //check if the last line is empty, if so, remove it
    if(lines[lines.length-1] == "") {
        lines = lines.slice(0,-1);
    }

    //first line is the attribute names
    var name = lines[0].split(",");
    var result = '[';

    for(var i = 1; i < lines.length; i++){
        var line = lines[i].split(",");
        result += '{';
        for(var j = 0; j < name.length; j++){
            result += '"'+ name[j] + '":"' + line[j] + '",';
        }
        result = result.slice(0,-1);
        result += '},';
    }
    //exclude the last ,
    result = result.slice(0,-1);
    //add the ]
    result += ']';
    return JSON.parse(result);
}