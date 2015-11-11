Pebble.addEventListener('ready',
    function(e) {
        console.log('JavaScript app ready and running!');
    }
);

Pebble.addEventListener('showConfiguration', function(e) {
    // Show config page
    Pebble.openURL('https://rabits.org/pebble/flux-capacitor/configuration_14.html?_=' + new Date().getTime());
});

Pebble.addEventListener('webviewclosed', function(e) {
    // Decode and parse config data as JSON
    var config_data = JSON.parse(decodeURIComponent(e.response));
    console.log('Config window returned: ' + JSON.stringify(config_data));

    // Pebble time int32 limitations
    var dest = config_data['DESTINATION']
    var dest_64 = false
    var dest_h = 0
    if( dest < -2147483648 || dest > 2147483647 ) {
        // Split int64 into 2xint32 and send to pebble separately
        dest_64 = true
        var dest_negative = dest < 0

        dest = ("0000000000000000000000000000000000000000000000000000000000000000" + Math.abs(dest + (dest_negative ? 1 : 0)).toString(2)).substr(-64)
        if( dest_negative ) {
            var temp = dest.substr(-63)
            dest = "1"
            for (i in temp) { dest += temp[i] ^ 1 }
        }

        dest_h = parseInt(dest.substr(0, dest.length-32), 2)
        dest = parseInt(dest.substr(-32), 2)
    }

    // Prepare AppMessage payload
    var data = {
        'KEY_ANIMATION': config_data['ANIMATION'],
        'KEY_DESTINATION': dest,
        'KEY_DESTINATION_USE64': dest_64,
        'KEY_DESTINATION_H': dest_h,
        'KEY_TIMEMACHINE': config_data['TIMEMACHINE'],
        'KEY_TIMEMACHINE_LOCK': config_data['TIMEMACHINE_LOCK']
    };

    // Send settings to Pebble watchapp
    Pebble.sendAppMessage(data, function(){
        console.log('Sent config data to Pebble ' + JSON.stringify(data));
    }, function() {
        console.log('Failed to send config data! ' + JSON.stringify(data));
    });
});
