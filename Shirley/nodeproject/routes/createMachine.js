var myDir = require("./myDir");
var fs = require("fs");

module.exports = function(src,dest,request,response){
    myDir.myCopyFiles(src,dest,{},function(err){
        if(err) throw err;
        else{
            fs.mkdir(dest+"/RESULTS",function(err){
                if(err) throw err;
                else{
                    fs.writeFile(dest+"/RESULTS/DOWNTIME.TXT","",function(err){
                        if(err) throw err;
                        else{
                            fs.readFile(src+"/machine_monitor_setup.csv",function(err,content){
                                if(err) throw err;
                                else{
                                    var currentPart = content.toString().split("\r\n")[0].split(",")[1];
                                    myDir.myCopyFiles(src+"/"+currentPart,dest+"/"+currentPart,{},function(err){
                                        if(err) throw err;
                                        else{
                                            fs.mkdir(dest+"/"+currentPart+"/RESULTS",function(err){
                                                if(err) throw err;
                                                else{
                                                    fs.writeFile(dest+"/"+currentPart+"/RESULTS/ALARMSUM.TXT","",function(err){
                                                        if(err) throw err;
                                                        else{
                                                            fs.writeFile(dest+"/"+currentPart+"/RESULTS/GRAPHLST.TXT","",function(err){
                                                                if(err) throw err;
                                                                else{
                                                                    fs.writeFile(dest+"/"+currentPart+"/RESULTS/SHOTPARM.CSV","",function(err){
                                                                        if(err) throw err;
                                                                        else{
                                                                            response.end();
                                                                        }
                                                                    });
                                                                }
                                                            });
                                                        }
                                                    });
                                                }
                                            });
                                        }

                                    });
                                }
                            });
                        }
                    });
                }
            });
        }
    });
}
