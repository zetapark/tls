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
		alert('mail sent');
		event.preventDefault();
		$.post('https://misc.zeta2374.com/leave_message', $('form').serialize(),
			function(data) { alert(data); });
	});
	$( '#design-request' ).submit( function( e ) {
			$.ajax( {
				url: 'design.php',
				type: 'POST',
				data: new FormData( this ),
				processData: false,
				contentType: false,
				success: function(data) {
					alert(data);
				}
			} );
			e.preventDefault();
		} );
});
