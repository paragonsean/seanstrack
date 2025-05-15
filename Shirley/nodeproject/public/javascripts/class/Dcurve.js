function Dcurve(arr){
    this.index = arr[0];
    this.name = arr[1];
    this.headCoef = arr[2];
    this.rodCoef = arr[3];
}

/*******************************************************************
 *                         getValStr                             *
 ********************************************************************/
Dcurve.prototype.getValStr = function(){
    return this.index+","+this.name+","+this.headCoef+","+this.rodCoef;
}

/*******************************************************************
 *                         copy                             *
 ********************************************************************/
Dcurve.copy = function(){
    var valueStr = this.getValStr();
    var newDcurve = new Dcurve(valueStr.split(","));

    return newDcurve;
}