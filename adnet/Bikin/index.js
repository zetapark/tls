$(function() {
	$('#lang').click(function() {
		$.get('lang.php', function(data) { console.log(data); });
		$('.eng').toggle();
		$('.kor').toggle();
	});
	$('#search').click(function() {
		$.post('search.php', {search : $('#search_text').val()}, function(data) {
			$('#div1').html(data);
		});
	});
	$('#contact-form').submit(function(event) {
		event.preventDefault();
		$.post('https://misc.zeta2374.com/leave_message', $('form').serialize(),
			function(data) { alert(data); });
	});
});
