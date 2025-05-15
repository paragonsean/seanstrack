//translations
//for multiple languages
//this replace english with the language where the ID is
/*******************************************************************
 *                          translation                        *
 ********************************************************************/
var translation = function(xmlpath,language) {
    $.ajax({
        url: xmlpath,
        success: function(xml) {
            $(xml).find('translation').each(function(){
                var id = $(this).attr('id');
                var text = $(this).find(language).text();
                $("#" + id).html(text);
            });
        }
    });
}