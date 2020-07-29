$(function() {
    navigator.geolocation.getCurrentPosition(function(position) {
			$('#demo').html("Latitude: " + position.coords.latitude + "<br>Longitude: "
				+ position.coords.longitude);
		});
	$.get("http://ipinfo.io", function(response) {
		$('#demo').html(response.city + response.region + response.country);
	}, "jsonp");
});
