//analog sensor class
/***********************************************************************
 *                         ANALOG_SENSOR_CLASS                          *
 ***********************************************************************/
function Analog_sensor(desc, units, vartype, low_volts, low_value, high_volts, high_value, points){
    this.desc = desc;
    this.units = units;
    this.vartype = vartype;
    this.low_volts = low_volts;
    this.high_volts = high_volts;
    this.low_value = low_value;
    this.high_value = high_value;
    this.points = points;  //all the points user enter, including max/min values

    this.max_volts_level = 0; // not used
    this.max_volts = 0; // not used
    this.m = [];  ///not saved
    this.b = [];   ///not saved
}

/*******************************************************************
 *                         getValStr                              *
 ********************************************************************/
//get a string of values with comma separated
//points are included but points are also comma seperated
//this is used for saving the values to the file
Analog_sensor.prototype.getValStr = function(){
    var points = JSON.stringify(this.points).replace(/,/g," ").replace(/\]\]/g,"]").replace(/\[\[/g,"[").replace(/\] \[/g,"],[");
    return this.desc + "," + this.units + "," + this.vartype + "," + this.low_volts + "," + this.low_value + "," +
        this.high_volts + "," + this.high_value + "," + points;
}
