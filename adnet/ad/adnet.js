function request_ad() {
	if(document.hasFocus() && $('#adnet2').is(':visible')) {
		$.post(
			'https://ad.zeta2374.com/request_ad.php',
			{ id : $('#adnet2').attr('myid'), category : $('#adnet2').attr('category'), 
				url : window.location.href },
			function(data) {
				var split = data.split('\n');
				$('#adnet2').attr('src', 'https://tomcat.zeta2374.com/banner/' + split[0] 
					+ '-' + $('#adnet2').attr('adtype') + '.jpg');//img
				$('#adnet2').off('click');
				$('#adnet2').click(function() {
					$.post('https://ad.zeta2374.com/click_ad.php',
						{id : $('#adnet2').attr('myid'), click : split[0], token : split[2]});//id
					window.open(split[1], '_blank');//link
				});
			}
		);
	}
}

$(function() {
	var tmp = $('#adnet2').attr('src');
	$('#adnet2').on('error', function() {
		$('#adnet2').on('error', function() {
			$('#adnet2').attr('src', tmp);
			$('#adnet2').off('click');
		});
		$('#adnet2').attr('src', 'https://tomcat.zeta2374.com/banner/adnet2-' +
			$('#adnet2').attr('adtype') + '.jpg');
		$('#adnet2').off('click');
		$('#adnet2').click(function() {
			$.post('https://ad.zeta2374.com/click_ad.php', {id : $('#adnet2').attr('myid'),
				click : 'adnet2'});
			window.open('https://adnet2.zeta2374.com', '_blank');
		});
	});

	$('#adnet2').bind('contextmenu', function() {
		window.open('https://adnet2.zeta2374.com', '_blank');
	});

	request_ad();
	var interval = parseInt($('#adnet2').attr('interval'), 10) || 0;
	if(interval < 10) interval = 10;
	setInterval(request_ad, interval * 1000);
	$('#adnet2').css('cursor', 'pointer');
});
