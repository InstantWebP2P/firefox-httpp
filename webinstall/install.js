require.paths.unshift('./node_modules/');

var connect = require('connect');

 var app = connect()
    .use(connect.logger('dev'))
    .use(connect.static('pub'))
    .use(function(req, res){
        res.end('hello world\n');
    });

console.log('Http connect server running on TCP port 51686 ...');
app.listen(51686);

