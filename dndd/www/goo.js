var map;
var service;
var infowindow;
var placeSearch, autocomplete;
var componentForm = {
    street_number: 'short_name',
    route: 'long_name',
    locality: 'long_name',
    administrative_area_level_1: 'short_name',
    country: 'long_name',
    postal_code: 'short_name'
};
function initialize() {
    var pyrmont = new google.maps.LatLng(-33.8665433, 151.1956316);
    map = new google.maps.Map(document.getElementById('map'), {
        center: pyrmont,
        zoom: 15
    });
}
function myclick() {
    var request = { bounds: map.getBounds(), query: $('#name').val() };
    service = new google.maps.places.PlacesService(map);
    service.textSearch(request, callback);
}
var marker = [];
var info = [];
function callback(results, status) {
    if (status == google.maps.places.PlacesServiceStatus.OK) {
        $.post('googleapi', { 'json': JSON.stringify(results) }, function (data, status) { });
        var _loop_1 = function (i) {
            marker[i] = new google.maps.Marker({ position: results[i].geometry.location, map: map });
            marker[i].setLabel(results[i].name);
            info[i] = new google.maps.InfoWindow({ content: results[i].name });
            marker[i].addListener('click', function () { info[i].open(map, marker[i]); });
        };
        for (var i = 0; i < results.length; i++) {
            _loop_1(i);
        }
    }
}
function initAutocomplete() {
    autocomplete = new google.maps.places.Autocomplete((document.getElementById('autocomplete')), { types: ['geocode'] });
    autocomplete.addListener('place_changed', fillInAddress);
}
function fillInAddress() {
    // Get the place details from the autocomplete object.
    var place = autocomplete.getPlace();
    map.setCenter(place.geometry.location);
    $.post('googleapi', { 'json': JSON.stringify(place) }, function (data, status) { });
    for (var component in componentForm) {
        document.getElementById(component).value = '';
        document.getElementById(component).disabled = false;
    }
    // Get each component of the address from the place details
    // and fill the corresponding field on the form.
    for (var i = 0; i < place.address_components.length; i++) {
        var addressType = place.address_components[i].types[0];
        if (componentForm[addressType]) {
            var val = place.address_components[i][componentForm[addressType]];
            document.getElementById(addressType).value = val;
        }
    }
}
function geolocate() {
    if (navigator.geolocation) {
        navigator.geolocation.getCurrentPosition(function (position) {
            var geolocation = {
                lat: position.coords.latitude,
                lng: position.coords.longitude
            };
            var circle = new google.maps.Circle({
                center: geolocation,
                radius: position.coords.accuracy
            });
            autocomplete.setBounds(circle.getBounds());
        });
    }
}
var origin, destination;
var ways = [];
function start() {
    origin = map.getCenter();
    new google.maps.Marker({ position: origin, map: map, label: 'S' });
}
function end() {
    destination = map.getCenter();
    new google.maps.Marker({ position: destination, map: map, label: 'E' });
}
function waypoint() {
    var center = map.getCenter();
    ways.push({ location: center, stopover: true });
    new google.maps.Marker({ position: center, map: map, label: 'w' });
}
function find() {
    var directionsDisplay = new google.maps.DirectionsRenderer();
    var directionsService = new google.maps.DirectionsService();
    directionsDisplay.setMap(map);
    var request = { origin: origin, destination: destination, waypoints: ways,
        travelMode: 'DRIVING', optimizeWaypoints: true };
    directionsService.route(request, function (result, status) {
        if (status == 'OK') {
            var jv = result.routes[0].legs;
            for (var i = 0; i < jv.length; i++) {
                var x = jv[i].end_location.lat;
                var y = jv[i].end_location.lng;
                var dist = 10000;
                var w1 = start, w2, w;
                if (i == jv.length - 1)
                    w2 = end;
                else {
                    for (var ww in ways) {
                        w = ww.location;
                        console.log(x);
                        console.log(y);
                        console.log(w);
                        var d = (x - w.lat) * (x - w.lat) + (y - w.lng) * (y - w.lng);
                        if (d < dist) {
                            dist = d;
                            w2 = w;
                        }
                    }
                }
                directionsService.route({ origin: w1, destination: w2, travelMode: 'TRANSIT' }, function (r, status) { directionsDisplay.setDirections(r); });
                w1 = w2;
            }
            $.post('googleapi', { 'json': JSON.stringify(result), 'direction': '1' }, function (data, status) { });
        }
    });
}
$(function () {
    initialize();
    initAutocomplete();
});
