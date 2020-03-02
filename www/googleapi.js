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

//initialize
$(function() {
	var pyrmont = new google.maps.LatLng(-33.8665433,151.1956316);
	map = new google.maps.Map(document.getElementById('map'), {
		center: pyrmont,
		zoom: 15
	});

	autocomplete = new google.maps.places.Autocomplete(
			(document.getElementById('autocomplete')), {types: ['geocode']});
	autocomplete.addListener('place_changed', fillInAddress);
})

//to search with text query
function myclick() {
	var request = { bounds: map.getBounds(), query: $('#name').val() };
	service = new google.maps.places.PlacesService(map);
	service.textSearch(request, callback);
}

var marker = []
var info = []
function callback(results, status) {
	if(status == google.maps.places.PlacesServiceStatus.OK) {
		$.post('googleapi', {'json':JSON.stringify(results)}, function(data,status){});
		for(let i =0; i<results.length; i++) {
			marker[i] = new google.maps.Marker({position : results[i].geometry.location, map:map});
			marker[i].setLabel(results[i].name);
			info[i] = new google.maps.InfoWindow({ content:results[i].name });
			marker[i].addListener('click', function() { info[i].open(map,marker[i]); }); 
		}
	}
} 

//to get address with autocomplete
function fillInAddress() {
	// Get the place details from the autocomplete object.
	var place = autocomplete.getPlace();
	map.setCenter(place.geometry.location);
	$.post('googleapi', {'json':JSON.stringify(place)}, function(data, status){});

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
		navigator.geolocation.getCurrentPosition(function(position) {
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

//to insert travel positions
var origin, destination;
var ways = [];
function start() {//start button click
	origin = map.getCenter();
	new google.maps.Marker({position:origin, map:map, label:'S'});
}
function end() {//end button click
	destination = map.getCenter();
	new google.maps.Marker({position:destination, map:map, label:'E'});
}
function waypoint() {//waypoint button click
	var center = map.getCenter();
	ways.push({location:center, stopover:true });
	new google.maps.Marker({position:center, map:map, label:'w'});
}

//to draw transit with waypoints
var directionsService = new google.maps.DirectionsService();
function find() {
	var request = { origin: origin, destination: destination, waypoints: ways,
		travelMode: 'DRIVING', optimizeWaypoints: true };
	directionsService.route(request, optimize); 
}

function optimize(result, status) {
	var jv = result.routes[0].legs;
	var w1=origin, w2;
	for(var i=0; i<jv.length; i++) {
		var dist = 10000;
		if(i == jv.length-1) w2 = destination;
		else for(var k=0; k<ways.length; k++) {
			var d = getDistance(jv[i].end_location, ways[k].location);
			if(d < dist) {
				dist = d;
				w2 = ways[k].location;
			}
		}
		directionsService.route({origin:w1,destination:w2, travelMode:'BICYCLING'}, display);
		w1 = w2;
	}
}

var rad = function(x) { return x * Math.PI / 180; };
var getDistance = function(p1, p2) {
  var R = 6378137; // Earth’s mean radius in meter
  var dLat = rad(p2.lat() - p1.lat());
  var dLong = rad(p2.lng() - p1.lng());
  var a = Math.sin(dLat / 2) * Math.sin(dLat / 2) +
    Math.cos(rad(p1.lat())) * Math.cos(rad(p2.lat())) *
    Math.sin(dLong / 2) * Math.sin(dLong / 2);
  var c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
  var d = R * c;
  return d; // returns the distance in meter
};

function display(result, status) {
	var	directionsDisplay = new google.maps.DirectionsRenderer();
	directionsDisplay.setMap(map);
	directionsDisplay.setDirections(result); 
	$.post('googleapi', {'json':JSON.stringify(result), 'direction':'1'},
			function(data, status){});
}

function save() {
	html2canvas(document.querySelector("#map")).then(function(canvas) {document.body.appendChild(canvas)});
}

