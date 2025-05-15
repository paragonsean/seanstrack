var fs = require('fs');
var path = require('path');

module.exports = function(srcpath, request, response) {

    fs.readdir(srcpath, function(err, files){

            if(err) throw err;
            else{
                var i = 0;
                var dirs = '';
                //start the for loop parallel
                files.forEach(function(file){
                        fs.stat(path.join(srcpath, file), function (err, stats) {
                            if (err) throw err;
                            else {
                                setTimeout(function() {
                                    if (stats.isDirectory()) {
                                        dirs += file + ",";
                                    }
                                    i++;

                                    if (i == files.length) {
                                        //console.log(dirs);
                                        response.write(dirs.slice(0,-1)); //remove the last ,
                                        response.end();
                                    }
                                 });
                            }
                        });


                });
            }


	});
   
}