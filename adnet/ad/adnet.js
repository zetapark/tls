function request_ad() {
	if(document.hasFocus() && $('#ad').is(':visible')) {
		$.post(
			'https://ad.zeta2374.com/request_ad.php',
			{ id : $('#ad').attr('myid'), category : $('#ad').attr('category'), 
				url : window.location.href },
			function(data) {
				var split = data.split('\n');
				$('#ad').attr('src', 'https://tomcat.zeta2374.com/banner/' + split[0] 
					+ '-' + $('#ad').attr('type') + '.jpg');//img
				$('#ad').off('click');
				$('#ad').click(function() {
					$.post('https://ad.zeta2374.com/click_ad.php',
						{id : $('#ad').attr('myid'), click : split[0], token : split[2]});//id
					window.open(split[1], '_blank');//link
				});
			}
		);
	}
}

$(function() {
	var tmp = $('#ad').attr('src');
	$('#ad').on('error', function() {
		$('#ad').on('error', function() {
			$('#ad').attr('src', tmp);
			$('#ad').off('click');
		});
		$('#ad').attr('src', 'https://tomcat.zeta2374.com/banner/ad-' +
			$('#ad').attr('type') + '.jpg');
		$('#ad').off('click');
		$('#ad').click(function() {
			$.post('https://ad.zeta2374.com/click_ad.php', {id : $('#ad').attr('myid'),
				click : 'ad'});
			window.open('https://ad.zeta2374.com', '_blank');
		});
	});

	$('#ad').bind('contextmenu', function() {
		window.open('https://adnet.zeta2374.com', '_blank');
	});

	request_ad();
	var interval = parseInt($('#ad').attr('interval'), 10) || 0;
	if(interval < 10) interval = 10;
	setInterval(request_ad, interval * 1000);
	$('#ad').css('cursor', 'pointer');
});
