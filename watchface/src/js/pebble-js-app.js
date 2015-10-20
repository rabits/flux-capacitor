Pebble.addEventListener('ready',
    function(e) {
        console.log('JavaScript app ready and running!');
    }
);

Pebble.addEventListener('showConfiguration', function(e) {
    // Show config page
    Pebble.openURL('http://rabits.org/pebble/flux-capacitor/configuration.html');
});

Pebble.addEventListener('webviewclosed', function(e) {
  // Decode and parse config data as JSON
  var config_data = JSON.parse(decodeURIComponent(e.response));
  console.log('Config window returned: ', JSON.stringify(config_data));

  // Prepare AppMessage payload
  var dict = {
    'KEY_ANIMATION': config_data['ANIMATION']
  };

  // Send settings to Pebble watchapp
  Pebble.sendAppMessage(dict, function(){
    console.log('Sent config data to Pebble');  
  }, function() {
    console.log('Failed to send config data!');
  });
});
