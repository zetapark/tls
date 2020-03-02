function b64e(s) {
	return window.btoa(unescape(encodeURIComponent(s)));
}
function b64d(s) {
	return decodeURIComponent(escape(window.atob(s)));
}
function encode_before_submit() {
	$("#content").val(b64e($("#content").val()));
}
$(function on_ready() {
	$('#content').val(b64d($('#content').val()));
});
