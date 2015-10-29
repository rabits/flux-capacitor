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
    var dest = Math.max(-2147483648 + 3600*12, Math.min(2147483647 - 3600*12, config_data['DESTINATION']))

    // Prepare AppMessage payload
    var data = {
        'KEY_ANIMATION': config_data['ANIMATION'],
        'KEY_DESTINATION': dest,
        'KEY_TIMEMACHINE': config_data['TIMEMACHINE']
    };

    // Send settings to Pebble watchapp
    Pebble.sendAppMessage(data, function(){
        console.log('Sent config data to Pebble ' + JSON.stringify(data));
    }, function() {
        console.log('Failed to send config data! ' + JSON.stringify(data));
    });
});
