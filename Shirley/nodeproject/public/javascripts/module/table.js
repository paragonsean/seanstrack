//functions for make a table
function makeOneColumnTable(array){
    var result = "";
    var len = array.length;
    for (var i=0; i<len; i++){
        result += "<tr><td>" + array[i] + "</td></tr>"
    }

    return result;
}

//make a whole table selectable
//select a row and return row ID
//input tableID in html
//ini function
//no headers
function selectRow(tableID,ini){
    var rows = document.getElementById(tableID).getElementsByTagName("tr");
    for (var i = 0; i < rows.length; i++){
        var row = rows[i];
        row.addEventListener("click",function(){
            $("#" + tableID + " tr").not(this).css('background-color','white');
            $(this).css('background-color','LightGray');
            ini(this.rowIndex);
        });
    }
}

//select table with headers
function selectRowWH(tableID,ini){
    var rows = document.getElementById(tableID).getElementsByTagName("tr");
    for (var i = 1; i < rows.length; i++){
        var row = rows[i];
        row.addEventListener("click",function(){
            $("#" + tableID + " tr").not(this).css('background-color','white');
            $(this).css('background-color','LightGray');
            ini(this.rowIndex-1);
        });
    }
}

//add new row to table and make it selectable
function addRowOneColumn(tableID,rowID,content,ini){
    var table = document.getElementById(tableID);
    var newRow = table.insertRow(rowID);
    newRow.insertCell(0).innerHTML = content;
    newRow.addEventListener("click", function() {
        $("#" + tableID + " tr").not(this).css('background-color','white');
        $(this).css('background-color','LightGray');
        ini(rowID);
    });
}

//function to make a x row y column table, editable
function makeXYTableEdit(x,y,array){
    var result = "";
    for (var i = 0; i < x; i++){
        result += "<tr>";
        for (var j = 0; j < y; j++){
            result += "<td contenteditable>" + array[i][j] + "</td>";
        }
        result += "</tr>";
    }
    return result;
}

//function to delete a row
function deleteRow(tableID,deleteRowID,clickRowID){
    var table = document.getElementById(tableID);
    table.deleteRow(deleteRowID);
    table.getElementsByTagName("tr")[clickRowID].click();
}

//copy selected row and insert a row below it
//select the new row
function insertRowWH(tableID,rowID,ini){
    var table = document.getElementById(tableID);
    var newRow = table.insertRow(rowID+1);
    var copyRow = table.rows[rowID];
    for (var i = 0, col; col = copyRow.cells[i]; i++){
        var newCell =  newRow.insertCell(i);
        newCell.innerHTML = col.innerHTML.toString();
        newCell.setAttribute('contenteditable','true');
    }
    newRow.addEventListener("click", function() {
        $("#" + tableID + " tr").not(this).css('background-color','white');
        $(this).css('background-color','LightGray');
        ini(newRow.rowIndex-1);
    });
}

//save a table
//comma seperated string
//newline character separated
function saveTableWH(tableID){
    var table = document.getElementById(tableID);
    var result = "";
    for(var i = 1, row; row = table.rows[i]; i++){
        for (var j = 0, col; col = row.cells[j]; j++){
            result += col.innerHTML.toString() + ",";
            console.log(encodeURIComponent(col.innerHTML.toString()));
        }
        result = result.slice(0,-1);
    }
    result = result.slice(0,-1);
    return result;
}