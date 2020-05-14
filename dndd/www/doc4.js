$(function() {
	$('button').click(function() {
		$.post("result_view", {abc : 123, day : 'mon'}, function(data, status) {
			alert("Data : " + data + "\nStatus : " + status);
		});
	});
});
