function request_ad() {
	if(document.hasFocus()) {
		$.post('https://ad.zeta2374.com/request_ad.php', { id : $('#adnet').attr('myid') },
			function(data) {
				var split = data.split('\n');
				$('#adnet').attr('src', 'https://tomcat.zeta2374.com/banner/' + split[0] 
					+ '-' + $('#adnet').attr('adtype') + '.jpg');//img
				$('#adnet').off('click');
				$('#adnet').click(function() {
					$.post('https://ad.zeta2374.com/click_ad.php', {id : $('#adnet').attr('myid'),
						click : split[0], token : split[2]});//id
					window.open(split[1], '_blank');//link
				});
			}
		);
	}
}

$(function() {
	$('#adnet').on('error', function() {
		$('#adnet').attr('src', 'https://tomcat.zeta2374.com/banner/adnet-' +
			$('#adnet').attr('adtype') + '.jpg');
		$('#adnet').off('click');
		$('#adnet').click(function() {
			$.post('https://ad.zeta2374.com/click_ad.php', {id : $('#adnet').attr('myid'),
				click : 'adnet'});
			window.open('https://adnet.zeta2374.com', '_blank');
		});
	});

	request_ad();
	setInterval(request_ad, 10000);
	$('#adnet').css('cursor', 'pointer');
});
