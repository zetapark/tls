var table_ = '';
var book_;
var page_;
var group_ = '';
var email_ = '';
var name_ = '';
var level_;

$(document).on('click', '.menu', function() {
	table_ = $(this).text();
	replace.innerHTML = '';
	$.get("table.cgi?table=" + table_ + "&group=" + group_, function(data, status) {
		var vs = data.split("\n");
		for(var i=0; i<vs.length-1; i+=2) {
			replace.innerHTML += "<li class='list-group-item list-group-item-success'><a href='#'><i>" + vs[i] + "</i>. " + vs[i+1] + "</a></li>";
		}
	});
});

$(document).on('click', '.list-group-item-success', function() {
	book_ = $(this).find("i").text();
	replace.innerHTML = '';
	$.get("book.cgi?group=" + group_ + "&table=" + table_ + "&book=" + book_, function(data, status) {
		var vs = data.split("\n");
		for(var i=0; i<vs.length-1; i+=2) {
			replace.innerHTML += "<li class='list-group-item list-group-item-secondary'><a href='#'><i>" + vs[i] + "</i>. " + vs[i+1] + "</a></li>";
		}
	});

});

$(document).on('click', '.list-group-item-secondary', function() {
	page_ = $(this).find("i").text();
	$.get("page.cgi?group=" + group_, "&table=" + table_ + "&book=" + book_ + "&page=" + page_, function(data, status) {
		var vs = eval('('+data+')');
		mt.innerHTML = vs[0]['title'];
		mb.innerHTML = vs[0]['text'];
		
		page_content.visible = 'block';
		var new_win = window.open('page.html');
		//new_win.document.write(vs[0]['text']);
		//for(var i=0; i<vs.length; i++) 
		new_win.onload = function () {
			new_win.document.tt.innerHTML = 'fdfd';
			//new_win.document.tt.innerHTML = 'fdsf';//getElementsById('tt').html(vs[0]['title']);
		};
		new_win.select();
	});
});

function login() {
	var json = {
		id : email.value,
		password : password.value,
		group : msg.value
	};
	replace.innerHTML = 'login';
	$.post("login.cgi", json, function(data, status) {
		if(data != "login failed") {
			$(".input-group").hide();
			var vs = data.split("\n");
			email_ = vs[0];
			level_ = parseInt(vs[1]);
			group_ = msg.value;
			document.cookie = "id=" + email_;
			document.cookie = "level=" + vs[1];
			for(var i=2; i<vs.length; i++) 
				li.innerHTML += "<li class='nav-item'><a href='#' class='menu'>"+ vs[i] + "</a></li>";
			logged.innerHTML = email_ + "님 반갑습니다.";
		} else replace.innerHTML = data;
	});				
}

