var configuration = {
    "forecast_key": "",
    "degrees": "f",
    "refresh": 30,
    "low_power": 20,
    "low_refresh": 60
};

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
    xhr.open (type, url);
    xhr.send ();
};

function locationSuccess(pos) {    
    // Construct URL    
    var url = "https://api.forecast.io/forecast/"+ configuration.forecast_key +"/" +
        pos.coords.latitude +","+ pos.coords.longitude;

    // Send request to OpenWeatherMap
    xhrRequest(url, 'GET', function(responseText) {
        var json = JSON.parse (responseText);
        
        // Temperature in Kelvin requires adjustment
        var temperature = Math.round (json.currently.temperature);
        var conditions = json.currently.icon;
        
        if (configuration.degrees == 'c') {
            temperature = (temperature - 32) / 1.8;
        }

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

function getConfiguration () {
    configuration.forecast_key = configuration.forecast_key = localStorage.getItem ("KEY_CONFIG_FORECAST_KEY");
    configuration.degrees = localStorage.getItem ("KEY_CONFIG_DEGREES");
    configuration.refresh = localStorage.getItem ("KEY_CONFIG_REFRESH");
    configuration.low_power = localStorage.getItem ("KEY_CONFIG_LOW_POWER");
    configuration.low_refresh = localStorage.getItem ("KEY_CONFIG_LOW_REFRESH");
}

function saveConfiguration (config) {
    var update_weather = false;
    
    if (typeof config !== "undefined" && config) {
        if (config.forecast_key && config.forecast_key.length > 0 && configuration.forecast_key != config.forecast_key) {
            update_weather = true;
        }
        
        configuration.forecast_key = config.forecast_key;
        configuration.degrees = config.degrees;
        configuration.refresh = config.refresh;
        configuration.low_power = config.low_power;
        configuration.low_refresh = config.low_refresh;
    }
    
    localStorage.setItem ("KEY_CONFIG_FORECAST_KEY", configuration.forecast_key);
    localStorage.setItem ("KEY_CONFIG_DEGREES", configuration.degrees);
    localStorage.setItem ("KEY_CONFIG_REFRESH", configuration.refresh);
    localStorage.setItem ("KEY_CONFIG_LOW_POWER", configuration.low_power);
    localStorage.setItem ("KEY_CONFIG_LOW_REFRESH", configuration.low_refresh);
    
    pushConfiguration ();
    
    if (update_weather) {
        getWeather ();
    }
}

// Sends the current configuration to the watch
function pushConfiguration () {
    // Assemble dictionary using our keys
    var dictionary = {
        "KEY_CONFIGURATION": 0,
        "KEY_CONFIG_FORECAST_KEY": configuration.forecast_key,
        "KEY_CONFIG_DEGREES": configuration.degrees,
        "KEY_CONFIG_REFRESH": configuration.refresh,
        "KEY_CONFIG_LOW_POWER": configuration.low_power,
        "KEY_CONFIG_LOW_REFRESH": configuration.low_refresh
    };

    // Send to Pebble
    Pebble.sendAppMessage(dictionary,
        function(e) {
            console.log("Configuration info sent to Pebble successfully!");
        },
        function(e) {
            console.log("Error sending configuration info to Pebble!");
        }
    );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', function(e) {
    getConfiguration ();
});

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage', function(e) {
    getWeather ();    // only receive weather right now
});

// Listen for pebble app configuration
Pebble.addEventListener('showConfiguration', function(e) {
    Pebble.openURL('http://projects.nsfugames.com/Pebble/AndroidHome.html?configuration=' + encodeURIComponent (JSON.stringify (configuration)));
});

// Wait for the webview to close, once it does pull out the configuration information
Pebble.addEventListener('webviewclosed',
    function(e) {
        var config = JSON.parse(decodeURIComponent(e.response));
        saveConfiguration (config);
    }
);