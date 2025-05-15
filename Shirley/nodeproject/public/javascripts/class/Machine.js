//machine Class
//read in the array of attribute values
function Machine(arr, name){
    this.name = name;
    this.machineType = arr[0];
    this.fasTrakBoardNumber = arr[1];
    this.isSureTrak = arr[2];
    this.rodPitch = arr[3];
    this.autoShotTimeout = arr[4];
    this.downtimeTimeout = arr[5];
    this.differentialCurveNumber = arr[6];
    this.positionBaseImpactPoints = arr[7];
    this.timeBaseImpactPoints = arr[8]
    this.postImpactPressureType = arr[9];
    this.operatorNumber = arr[10];
    this.quadratureDivisor = arr[11];
    this.binaryValvePulseTime = arr[12];
    this.binaryValveDeventTime = arr[13];
    this.binaryValvePulseWire = arr[14];
    this.valveSettingAfterJogShot = arr[15];
    this.isMonitoringOn = arr[16];
    this.isAlarmMonitoringOn = arr[17];
    this.hasDigitalServo = arr[18];
    this.hasPressureControl = arr[19];
    this.useTimeToMeasureBiscuit = arr[20];
    this.hasBinaryValve = arr[21];
    this.hasHMI = arr[22];
    this.lsOffAtEOS = arr[23];

}

/*******************************************************************
 *                         getValArr                              *
 ********************************************************************/
//get an array of attribute values with the sequence in the data file
Machine.prototype.getValArr = function(){
    var string =  this.machineType + "," +
    this.fasTrakBoardNumber + "," +
    this.isSureTrak + "," +
    this.rodPitch+ "," +
    this.autoShotTimeout + "," +
    this.downtimeTimeout + "," +
    this.differentialCurveNumber + "," +
    this.positionBaseImpactPoints + "," +
    this.timeBaseImpactPoints + "," +
    this.postImpactPressureType + "," +
    this.operatorNumber + "," +
    this.quadratureDivisor + "," +
    this.binaryValvePulseTime + "," +
    this.binaryValveDeventTime + "," +
    this.binaryValvePulseWire + "," +
    this.valveSettingAfterJogShot + "," +
    this.isMonitoringOn + "," +
    this.isAlarmMonitoringOn + "," +
    this.hasDigitalServo + "," +
    this.hasPressureControl + "," +
    this.useTimeToMeasureBiscuit + "," +
    this.hasBinaryValve + "," +
    this.hasHMI + "," +
    this.lsOffAtEOS;

    return string.split(",");

}

/*******************************************************************
 *                         getAttArr                              *
 ********************************************************************/
//get an array of attribute names with format in the file
//e.g. Machine Type, Fas Trak Board Number...
//this is for write the class to file
Machine.prototype.getAttArr = function(){
    var string = "Machine Type, Fast Trak Board Number, " +
        "Is Sure Trak Control,Rod Pitch," +
        "Auto Shot Timeout (Sec.), Downtime Timeout (Sec.), Differential Curve Number," +
        "Position Base Impact Points, Time Base Impact Points," +
        "Post Impact Pressure Type, Operator Number, Quadrature Divisor, Binary Valve Pulse Time," +
        "Binary Valve Devent Time, Binary Valve Pulse Wire, Valve Setting after Jog Shot," +
        "Is Monitoring On, Is Alarm Monitoring On, Has Digital Servo, Has Pressure Control, Use Time To Measure Biscuit" +
        "Has Binary Valve, Has HMI, LS Off At EOS";
    return string.split(",");
}

/*******************************************************************
 *                         copyAtt                              *
 ********************************************************************/
//this creates a new instance with a given name
Machine.prototype.copyAtt = function(name){
    var attArr = this.getValArr();
    var newMachine = new Machine(attArr,name);

    return newMachine;
}