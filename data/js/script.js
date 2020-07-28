
$(".btn").click(function()
{   
   var param = 
    {
        "tryb":    this.value,

        "hue":    $('[name="hue"]').val(),
        "alpha":  $('[name="alpha"]').val(),
        
        "hue2"  : $('[name="hue2"]').val(),
        "alpha2": $('[name="alpha2"]').val(),
        
        "s_val":  $('[name="s_val"]').val(),
        "i_val":  $('[name="i_val"]').val(),
        
        "r":  $('[name="r"]').val(),
        "g":  $('[name="r"]').val(),
        "b":  $('[name="b"]').val(),
    };
    
    $.post( "/set", param )
    .done(function( data, status ) 
    {
        $.each(data.dane, function(key,val) 
		{
            $('[name="'+key+'"]').val(val);
		});	
    });
});



$(function() 
{
    var colorPicker = new iro.ColorPicker('#color-picker-container', 
    {
        width: 280,
        borderWidth: 5,
        borderColor: "#fff",
    });

    function onColorChange(color, changes)
    {
        var rgb = color.rgb;
        $('[name="r"]').val(rgb["r"]);
        $('[name="g"]').val(rgb["g"]);
        $('[name="b"]').val(rgb["b"]);
    }

    colorPicker.on('color:change', onColorChange);

    var param =  {"tryb": 1 };
    $.post( "/set", param ) .done(function( data, status ) 
    {
        $.each(data, function(key,val){ $('[name="'+key+'"]').val(val);	});	
        colorPicker.color.rgb = { r: data["r"], g: data["g"], b: data["b"] };
    });
});