var fs = require("fs"),
    _path = require("path");

//copy directory recursively excluding the one matches opts
exports.myCopyDir = function myCopyDir(srcDir, newDir, opts, clbk) {
    var originalArguments = Array.prototype.slice.apply(arguments);
    srcDir = _path.normalize(srcDir);
    newDir = _path.normalize(newDir);

    fs.stat(newDir, function(err, newDirStat) {
        if(!err) {
            if(typeof opts !== 'undefined' && typeof opts !== 'function' && opts.forceDelete)
                return exports.rmdirRecursive(newDir, function(err) {
                    myCopyDir.apply(this, originalArguments);
                });
            else
                return clbk(new Error('You are trying to delete a directory that already exists. Specify forceDelete in an options object to override this.'));
        }

        if(typeof opts === 'function')
            clbk = opts;

        fs.stat(srcDir, function(err, srcDirStat){
            if (err) return clbk(err);

            fs.mkdir(newDir, srcDirStat.mode, function(err){
                if (err) return clbk(err);

                    fs.readdir(srcDir, function (err, files) {
                        if (err) return clbk(err);

                        (function copyFiles(err) {
                            if (err) return clbk(err);

                            var filename = files.shift();
                            if (filename === null || typeof filename == 'undefined')
                                return clbk(null);

                            var file = srcDir + '/' + filename,
                                newFile = newDir + '/' + filename;

                            fs.stat(file, function (err, fileStat) {
                                if (err) return clbk(err);

                                if (fileStat.isDirectory()){
                                    if(file.match(opts.exclude)) copyFiles();
                                    else  myCopyDir(file, newFile, copyFiles, clbk);
                                }

                                else if (fileStat.isSymbolicLink())
                                    fs.readlink(file, function (err, link) {
                                        if (err) return clbk(err);
                                        fs.symlink(link, newFile, copyFiles);
                                    });
                                else
                                    fs.readFile(file, function (err, data) {
                                        if (err) return clbk(err);
                                        fs.writeFile(newFile, data, copyFiles);
                                    });
                            });
                        })();
                    });
            });
        });
    });
};

//copy files in a folder excluding subdirectories
exports.myCopyFiles = function myCopyFiles(srcDir, newDir, opts, clbk) {
    var originalArguments = Array.prototype.slice.apply(arguments);
    srcDir = _path.normalize(srcDir);
    newDir = _path.normalize(newDir);

    fs.stat(newDir, function(err, newDirStat) {
        if(!err) {
            if(typeof opts !== 'undefined' && typeof opts !== 'function' && opts.forceDelete)
                return exports.rmdirRecursive(newDir, function(err) {
                    myCopyDir.apply(this, originalArguments);
                });
            else
                return clbk(new Error('You are trying to delete a directory that already exists. Specify forceDelete in an options object to override this.'));
        }

        if(typeof opts === 'function')
            clbk = opts;

        fs.stat(srcDir, function(err, srcDirStat){
            if (err) return clbk(err);

            fs.mkdir(newDir, srcDirStat.mode, function(err){
                if (err) return clbk(err);

                fs.readdir(srcDir, function (err, files) {
                    if (err) return clbk(err);

                    (function copyFiles(err) {
                        if (err) return clbk(err);

                        var filename = files.shift();
                        if (filename === null || typeof filename == 'undefined')
                            return clbk(null);

                        var file = srcDir + '/' + filename,
                            newFile = newDir + '/' + filename;

                        fs.stat(file, function (err, fileStat) {
                            if (err) return clbk(err);

                            if (fileStat.isDirectory()){
                                copyFiles();
                            }

                            else if (fileStat.isSymbolicLink())
                                fs.readlink(file, function (err, link) {
                                    if (err) return clbk(err);
                                    fs.symlink(link, newFile, copyFiles);
                                });
                            else
                                fs.readFile(file, function (err, data) {
                                    if (err) return clbk(err);
                                    fs.writeFile(newFile, data, copyFiles);
                                });
                        });
                    })();
                });
            });
        });
    });
};
