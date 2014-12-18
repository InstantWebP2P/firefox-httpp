var http = require("httpp");
var net = require("net");
var child = require("child_process");

var cmd = "./videosrv";
var args = "";
var options = null;

var app = http.createServer(function (req, res) {
    var date = new Date();
    res.writeHead(200, {
      'Date':date.toUTCString(),
      'Connection':'close',
      'Cache-Control':'private',
      'Content-Type':'video/webm',
      'Server':'CustomStreamer/0.0.1',
    });
    var server = net.createServer(function(socket){
        socket.on('data', function (data) {
          res.write(data);
        });
        socket.on('close', function (had_error) {
          res.end();
        });
      });
    server.listen(function(){
        args = [server.address().port, "/dev/video0"];
        console.log("muxer started on port : " + args);
        var gstMuxer = child.spawn(cmd, args, options);
        gstMuxer.stderr.on('data', onSpawnError);
        gstMuxer.on('exit', onSpawnExit);
        res.connection.on('close', function(){ gstMuxer.kill(); });
      });
  });

app.listen(5000);
console.log('Node-UDT stream server running on 5000');

function onSpawnError(data) {
  console.log(data.toString());
}
function onSpawnExit(code) {
  if (code != null) {
    console.error('gstreamer error, exit code'+ code);
  }
}
process.on('uncaughtException', function(err) {
  console.debug(err);
});
