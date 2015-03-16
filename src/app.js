var xhrRequest = function (url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.timeout = 5000;
    xhr.onload = function () { callback(this.responseText); };
    xhr.ontimeout = function () { 
        Pebble.sendAppMessage(
            { "KEY_TIMEOUT": 1 }, 
            function (e) {}, 
            function (e)  {}
        ); 
    };
    xhr.open(type, url);
    xhr.send();
};

function locationSuccess(pos) {
    // Construct URL    
    var url = "https://api.forecast.io/forecast/d480a4e2238ebafa1290f7422b788f5a/" +
        pos.coords.latitude +","+ pos.coords.longitude;
    
    console.log (url);

    // Send request to OpenWeatherMap
    xhrRequest(url, 'GET', function(responseText) {
        // responseText contains a JSON object with weather info
        var json = JSON.parse(responseText);
        

        // Temperature in Kelvin requires adjustment
        var temperature = Math.round (json.currently.temperature);
        console.log("Temperature is " + temperature);

        // Conditions
        var conditions = json.currently.icon;      
        console.log("Conditions are " + conditions);

        // Assemble dictionary using our keys
        var dictionary = {
            "KEY_WEATHER": 0,
            "KEY_TEMPERATURE": temperature,
            "KEY_CONDITIONS": conditions
        };

        // Send to Pebble
        Pebble.sendAppMessage(dictionary,
            function(e) {
                console.log("Weather info sent to Pebble successfully!");
            },
            function(e) {
                console.log("Error sending weather info to Pebble!");
            }
        );
    });
}

function locationError(err) {
    console.log("Error requesting location!");
}

function getWeather() {
    navigator.geolocation.getCurrentPosition(
        locationSuccess,
        locationError,
        {timeout: 15000, maximumAge: 60000}
    );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', function(e) {
    console.log("PebbleKit JS ready!");
});

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage', function(e) {
    console.log ("Got message: " + JSON.stringify(e));
    getWeather();
});
