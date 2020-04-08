var rect_pos = [50, 50, 50, 100, 100,100, 100, 50];
var point_idx = 8;

function on_mouse_down(canvas, event) {
	const rect = canvas.getBoundingClientRect()
	const x = event.clientX - rect.left
	const y = event.clientY - rect.top
	for(i=0; i<rect_pos.length; i+=2) {
		if(Math.abs(rect_pos[i] - x) < 10 && Math.abs(rect_pos[i+1] - y) < 10) point_idx = i;
	}
}

function draw_rect(canvas) {
	var ctx = canvas.getContext('2d');
	ctx.clearRect(0, 0, canvas.width, canvas.height);
	ctx.beginPath();
	ctx.moveTo(rect_pos[0], rect_pos[1]);
	for(i=2; i<8; i+=2) ctx.lineTo(rect_pos[i], rect_pos[i+1]);
	ctx.closePath();
	ctx.strokeStyle = '#0000ff';
	ctx.lineWidth = 2;
	ctx.stroke();
}

function on_mouse_up(canvas, event) {
	point_idx = 8;
}

function on_mouse_move(canvas, event) {
	if(point_idx != 8) {
		const rect = canvas.getBoundingClientRect()
		const x = event.clientX - rect.left
		const y = event.clientY - rect.top
		rect_pos[point_idx] = x;
		rect_pos[point_idx + 1] = y;
		draw_rect(canvas);
	}
}

$(function() {
	var c = document.getElementById('canv');
	var ctx = c.getContext('2d');
	c.addEventListener('mousedown', function(e) { on_mouse_down(c, e) }, false);
	c.addEventListener('mouseup', function(e) { on_mouse_up(c, e) }, false);
	c.addEventListener('mousemove', function(e) { on_mouse_move(c, e) }, false);
	draw_rect(c);
})

