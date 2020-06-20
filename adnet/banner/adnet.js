function request_ad() {
	$.post('https://ad.zeta2374.com/request_ad.php', { id : $('#adnet').attr('myid') },
		function(data) {
			var split = data.split('\n');
			$('#adnet').attr('src', 'https://tomcat.zeta2374.com/banner/' + split[0] 
					+ '-' + $('#adnet').attr('adtype') + '.jpg');//img
			$('#adnet').off('click');
			$('#adnet').click(function() {
				$.post('https://ad.zeta2374.com/click_ad.php', {id : $('#adnet').attr('myid'),
					click : split[0], token : split[2]});//id
				window.location.href = split[1];//link
			});
		}
	);
}

$(function() {
	$('#adnet').on('error', function() {
		$('#adnet').attr('src', 'https://tomcat.zeta2374.com/banner/adnet-' +
			$('#adnet').attr('adtype') + '.jpg');
		$('#adnet').off('click');
		$('#adnet').click(function() {
			$.post('https://ad.zeta2374.com/click_ad.php', {id : $('#adnet').attr('myid'),
				click : 'adnet'});
			window.location.href = 'https://adnet.zeta2374.com';
		});
	});

	request_ad();
	setInterval(request_ad, 10000);
});
