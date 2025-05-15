//machine monitor class
//read in the arry of attribute values
function MachineMonitor(arr,name){
    this.name = name;
    this.currentPartName = arr[0];
    this.profileSkipCount = arr[1];
    this.profileAcquireCount = arr[2];
    this.profileTimeSkip = arr[3];
    this.profileMaxSaved = arr[4];
    this.profileSkipCount = arr[5];
    this.profileAcquireAll = arr[6];
    this.profileAcquireNone = arr[7];
    this.paramSkipCount = arr[8];
    this.paramAcquireCount = arr[9];
    this.paramTimeSkip = arr[10];
    this.paramMaxSaved = arr[11];
    this.paramSkipCount = arr[12];
    this.paramSkipTime = arr[13];
    this.paramAcquireAll = arr[14];
    this.paramAcquireNone = arr[15];
}

/*******************************************************************
 *                         getValArr                              *
 ********************************************************************/
//get an array of attribute values with the sequence in the data file
MachineMonitor.prototype.getValArr = function(){
    var string =  this.currentPartName + "," +
    this.profileSkipCount + "," +
    this.profileAcquireCount + "," +
    this.profileTimeSkip + "," +
    this.profileMaxSaved + "," +
    this.profileSkipCount + "," +
    this.profileAcquireAll + "," +
    this.profileAcquireNone + "," +
    this.paramSkipCount + "," +
    this.paramAcquireCount + "," +
    this.paramTimeSkip + "," +
    this.paramMaxSaved + "," +
    this.paramSkipCount + "," +
    this.paramSkipTime + "," +
    this.paramAcquireAll + "," +
    this.paramAcquireNone;

    return string.split(",");
}

/*******************************************************************
 *                         getAttArr                              *
 ********************************************************************/
//get an array of attribute names with format in the file
//this is for write the class to file
MachineMonitor.prototype.getAttArr = function(){
    var string = "Current Part Name, Profile Skip Count, Profile Acquire Count, " +
        "Profile Time Skip (Sec.), Profile Max Saved, Profile Skip Count, " +
        "Profile Skip Time, Profile Acquire All, Profile Acquire None, " +
        "Param Skip Count, Param Acquire Count, Param Time Skip (Sec.), " +
        "Param Max Saved, Param Skip Count, Param Acquire All, Param Acquire None";
    return string.split(",");
}

/*******************************************************************
 *                         copyAtt                              *
 ********************************************************************/
//this creates a new instance with a given name
MachineMonitor.prototype.copyAtt = function(name){
    var attArr = this.getValArr();
    var newMachineMonitor = new MachineMonitor(attArr,name);

    return newMachineMonitor;
}