var xhrRequest = function (url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        callback(this.responseText);
    };
    xhr.open(type, url);
    xhr.send();
};

function locationSuccess(pos) {
    // Construct URL
    //var url = "http://api.openweathermap.org/data/2.5/weather?lat=" +
        //pos.coords.latitude + "&lon=" + pos.coords.longitude;
    
    var url = "https://api.forecast.io/forecast/d480a4e2238ebafa1290f7422b788f5a/39.911972,-82.971662";

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

    // Get the initial weather
    getWeather();
});

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage', function(e) {
    console.log ("Got message: " + JSON.stringify(e));
    getWeather();
});
