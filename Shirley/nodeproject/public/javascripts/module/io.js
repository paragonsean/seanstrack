//set and save info in html tags

/***********************************************************************
 *                         checkRadioWNameValue                         *
 ***********************************************************************/
function checkRadioWNameValue(name,value){
    $('input[name="'+name+'"][value="'+value+'"]').prop("checked",true);
}

/***********************************************************************
 *                         selectWIDValue                         *
 ***********************************************************************/
function selectWIDValue(id,value){
    $("#" + id + ">option[value='" + value + "']").prop("selected", true);
}

/***********************************************************************
 *                         checkBox                         *
 ***********************************************************************/
function checkBox(id,value){
    if(value=="T"){
        $("#" + id).prop('checked',true);
    }
    else{
        $("#" + id).prop("checked", false);
    }
}

/***********************************************************************
 *                         setTextbox                         *
 ***********************************************************************/
function setTextbox(id,value){
    $("#" + id).val(value);
}

/***********************************************************************
 *                         getRadioValueWName                         *
 ***********************************************************************/
function getRadioValueWName(name){
   return $("input[name='" + name + "']:checked").val();
}

/***********************************************************************
 *                         getValueWID                         *
 ***********************************************************************/
function getValueWID(id){
    return document.getElementById(id).value;
}

/***********************************************************************
 *                         getCheckboxValueWID                         *
 ***********************************************************************/
function getCheckboxValueWID(id){
    if(document.getElementById(id).checked){
        return "T";
    }
    else{
        return "F";
    }
}