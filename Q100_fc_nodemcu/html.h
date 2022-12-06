
const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html>
  <head>
    <title>Mobile remote</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0" />
    <style>
      html,
      body {
        overscroll-behavior-y: contain;
      }
      body {
        width: 100vw;
        height: 100vh;
        overflow: hidden;
        position: fixed;
        font-family: 'Gill Sans', 'Gill Sans MT', Calibri, 'Trebuchet MS', sans-serif;
        color: rgb(128, 128, 128);
        /* font-size: xx-large; */
      }
      .container {
        padding: 0;
      }
      .switch {
        position: relative;
        display: inline-block;
        width: 100px;
        height: 50px;
      }

      .switch input {
        opacity: 0;
        width: 0;
        height: 0;
      }

      .slider {
        position: absolute;
        cursor: pointer;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
        background-color: #ccc;
        -webkit-transition: 0.4s;
        transition: 0.4s;
      }

      .slider:before {
        position: absolute;
        content: '';
        height: 42px;
        width: 42px;
        left: 4px;
        bottom: 4px;
        background-color: white;
        -webkit-transition: 0.4s;
        transition: 0.4s;
      }

      input:checked + .slider {
        background-color: green;
      }

      input:focus + .slider {
        box-shadow: 0 0 1px green;
      }

      input:checked + .slider:before {
        -webkit-transform: translateX(46px);
        -ms-transform: translateX(46px);
        transform: translateX(46px);
      }
    </style>
  </head>
  <body scroll="no">
    <div class="container" style="height:100%;">
      <div style="display: flex; justify-content: center; padding: 5px">
        <label class="switch">
          <input type="checkbox" onchange="change()" id="power" />
          <span class="slider"></span>
        </label>
      </div>
      <p id="gyro" style="text-align:center;"></p>
      <div style="display: flex; justify-content: center; height: 100%">
        <div style="flex: 1; text-align: center">
          <p style="text-align: center; margin: 0">
            X: <span id="x_coordinate_left"> </span> Y: <span id="y_coordinate_left"> </span> Speed:
          </p>
          <canvas id="canvas_left" name="game"></canvas>
        </div>
        <div style="flex: 1; text-align: center">
          <p style="text-align: center; margin: 0">
            X: <span id="x_coordinate_right"> </span> Y: <span id="y_coordinate_right"> </span> Speed:
          </p>
          <canvas id="canvas_right" name="game"></canvas>
        </div>
      </div>
    </div>
      

    <script>
      var gateway = `ws://${window.location.hostname}/ws`;
      var websocket;
      var canvas_list = ['left', 'right'];
      var canvas = {};
      var ctx = {};
      var width,
        height,
        radius,
        x_orig = {},
        y_orig = {},
        x_relative = { left: 0, right: 50 },
        y_relative = { left: 0, right: 50 };
      var pwr = false;

      function addListeners(key) {
        canvas[key].addEventListener('mousedown', (event) => startDrawing(key, event));
        canvas[key].addEventListener('mouseup', (event) => stopDrawing(key, event));
        canvas[key].addEventListener('mousemove', (event) => moveHandler(key, event));
        canvas[key].addEventListener('touchstart', (event) => startDrawing(key, event), { passive: false });
        canvas[key].addEventListener('touchend', (event) => stopDrawing(key, event));
        canvas[key].addEventListener('touchcancel', (event) => stopDrawing(key, event));
        canvas[key].addEventListener('touchmove', (event) => moveHandler(key, event));
        document.getElementById('x_coordinate_left').innerText = x_relative['left'];
        document.getElementById('y_coordinate_left').innerText = y_relative['left'];
        document.getElementById('x_coordinate_right').innerText = x_relative['right'];
        document.getElementById('y_coordinate_right').innerText = y_relative['right'];
      }

      function initWebSocket() {
        console.log('Trying to open a WebSocket connection...');
        websocket = new WebSocket(gateway);
        websocket.onopen    = onOpen;
        websocket.onclose   = onClose;
        websocket.onmessage = onMessage; // <-- add this line
       }
      function onOpen(event) {
        console.log('Connection opened');
      }
    
      function onClose(event) {
        console.log('Connection closed');
        setTimeout(initWebSocket, 2000);
      }

      function onMessage(event) {
        console.log(event.data);
//          var state;
//          if (event.data == "1"){
//            state = "ON";
//          }
//          else{
//            state = d"OFF";
//          }
          document.getElementById('gyro').innerHTML = event.data;
      }

      function WS_send(x_,y_){  // PWR|T|P|R|Y
         websocket.send("remote:"+pwr+"|"+y_.left+"|"+y_.right+"|"+x_.right+"|"+x_.left);
      }
      
      window.addEventListener('load', () => {
        canvas.left = document.getElementById('canvas_left');
        canvas.right = document.getElementById('canvas_right');
        ctx.left = canvas.left.getContext('2d');
        ctx.right = canvas.right.getContext('2d');
        resize();
        Object.keys(canvas).map(addListeners);

        document.addEventListener('touchstart', () => {}, { passive: false });
        window.addEventListener('resize', resize);

        initWebSocket();
      });

      function resize() {
        width = 500;
        radius = 40;
        height = radius * 6.5;
        ctx.left.canvas.width = width;
        ctx.left.canvas.height = height;
        ctx.right.canvas.width = width;
        ctx.right.canvas.height = height;
        background('left', width / 2, height / 3);
        background('right', width / 2, height / 3);
        joystick('left', width / 2, height / 3);
        joystick('right', width / 2, height / 3);
      }

      function background(key) {
        x_orig[key] = width / 2;
        y_orig[key] = height / 3;

        ctx[key].beginPath();
        ctx[key].arc(width / 2, height / 3, radius + 20, 0, Math.PI * 2, true);
        ctx[key].fillStyle = '#ECE5E5';
        ctx[key].fill();
      }

      function joystick(key, x, y) {
        ctx[key].beginPath();
        ctx[key].arc(x, y, radius, 0, Math.PI * 2, true);
        ctx[key].fillStyle = '#F08080';
        ctx[key].fill();
        ctx[key].strokeStyle = '#F6ABAB';
        ctx[key].lineWidth = 8;
        ctx[key].stroke();
      }

      let coord = { left: { x: 0, y: 0 }, right: { x: 0, y: 0 } };
      let paint = { left: false, right: false };

      function getPosition(key, event) {
        var mouse_x = event.clientX;
        var mouse_y = event.clientY;
        coord[key].x = mouse_x - canvas[key].offsetLeft;
        coord[key].y = mouse_y - canvas[key].offsetTop;
      }

      function is_it_in_the_circle(key) {
        var current_radius = Math.sqrt(
          Math.pow(coord[key].x - x_orig[key], 2) + Math.pow(coord[key].y - y_orig[key], 2)
        );
        if (radius >= current_radius) return true;
        else return false;
      }

      function log(str) {
        document.getElementById('log').innerText = document.getElementById('log').innerText + ' |  ' + str;
      }

      var tpCache = new Array();

      function startDrawing(key, event) {
        event.preventDefault();
        if (event.touches && event.touches.length == 2) {
          for (var i = 0; i < event.touches.length; i++) {
            tpCache.push(event.touches[i]);
            let key = event.touches[i].target.id === 'canvas_left' ? 'left' : 'right';
            paint[key] = true;
            getPosition(key, event.touches[i]);
            if (is_it_in_the_circle(key)) {
              ctx[key].clearRect(0, 0, canvas[key].width, canvas[key].height);
              background(key);
              joystick(key, coord[key].x, coord[key].y);
              Draw(key, event.touches[i]);
            }
          }
        } else {
          paint[key] = true;
          getPosition(key, event.touches ? event.touches[0] : event);
          if (is_it_in_the_circle(key)) {
            ctx[key].clearRect(0, 0, canvas[key].width, canvas[key].height);
            background(key);
            joystick(key, coord[key].x, coord[key].y);
            Draw(key, event.touches ? event.touches[0] : event);
          }
        }
      }

      function stopDrawing(key, event) {
        event.preventDefault();
        if (tpCache.length == 2) {
          tpCache = [];
        }
        paint[key] = false;

        if (key === 'right') {
          ctx[key].clearRect(0, 0, canvas[key].width, canvas[key].height);
          background(key);
          joystick(key, width / 2, height / 3);
          document.getElementById('x_coordinate_' + key).innerText = 50;
          document.getElementById('y_coordinate_' + key).innerText = 50;
        }
      }

      function moveHandler(key, event) {
        if (tpCache.length == 2) {
          for (var i = 0; i < event.touches.length; i++) {
            let key = event.touches[i].target.id === 'canvas_left' ? 'left' : 'right';
            Draw(key, event.touches[i]);
          }
        } else {
          Draw(key, event.touches ? event.touches[0] : event);
        }
      }

      function deriveJoystickValues(value) {
        return (((value > 0 ? (radius - value) / 2 : (radius - value) / 2) / radius) * 100).toFixed(0);
      }

      function Draw(key, event) {
        if (paint[key]) {
          ctx[key].clearRect(0, 0, canvas[key].width, canvas[key].height);
          background(key);
          var x = { left: 0, right: 0 },
            y = { left: 0, right: 0 };
          var angle = Math.atan2(coord[key].y - y_orig[key], coord[key].x - x_orig[key]);

          if (is_it_in_the_circle(key)) {
            joystick(key, key === 'left' ? width / 2 : coord[key].x, coord[key].y);
            x[key] = key === 'left' ? width / 2 : coord[key].x;
            y[key] = coord[key].y;
          } else {
            x[key] = key === 'left' ? width / 2 : radius * Math.cos(angle) + x_orig[key];
            y[key] = radius * Math.sin(angle) + y_orig[key];
            joystick(key, x[key], y[key]);
          }

          getPosition(key, event);

          x_relative[key] = Math.round(x[key] - x_orig[key]);
          y_relative[key] = Math.round(y[key] - y_orig[key]);

          document.getElementById('x_coordinate_' + key).innerText = deriveJoystickValues(x_relative[key]);
          document.getElementById('y_coordinate_' + key).innerText = deriveJoystickValues(y_relative[key]);

          WS_send(x_relative, y_relative);
        }
      }

      function change() {
          pwr = document.getElementById('power').checked;
          document.body.style.backgroundColor = pwr ? 'black' : 'white';
          x_relative = { left: 0, right: 50 };
          y_relative = { left: 0, right: 50 };
          WS_send(x_relative,y_relative);
        }
    </script>
  </body>
</html>
<br />

)rawliteral";
