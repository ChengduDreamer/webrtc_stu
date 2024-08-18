// 001
var log4js = require('log4js');
var http = require('http');
var https = require('https');
var fs = require('fs');
var socketIo = require('socket.io');

var express = require('express');
var serveIndex = require('serve-index');

var USERCOUNT = 3;

log4js.configure({
    appenders: {
        file: {
            type: 'file',
            filename: 'app.log',
            layout: {
                type: 'pattern',
                pattern: '%r %p - %m',
            }
        }
    },
    categories: {
       default: {
          appenders: ['file'],
          level: 'debug'
       }
    }
});

var logger = log4js.getLogger();

var app = express();
app.use(serveIndex('./public'));
app.use(express.static('./public'));

 // 设置跨域访问 
app.all("*",function(req ,res ,next){ 
  // 设置允许跨域的域名， * 代表允许任意域名跨域 
	res.header("Access -Control -Allow -Origin","*"); 

	// 允许的header 类型 
	res.header("Access -Control -Allow -Headers","content -type"); 

	// 跨域允许的请求方式 
	res.header("Access -Control -Allow -Methods","DELETE ,PUT ,POST ,GET ,OPTIONS"); 
	if (req.method.toLowerCase () == 'options '){ 
		res.send (200); // 让options 尝试请求快速结束 
	}else { 
		next(); 
	} 
}); 

//http server
var http_server = http.createServer(app);
http_server.listen(8083, '0.0.0.0');

console.log("listen: 8083");

/*
var options = {
	key : fs.readFileSync('./cert/1557605_www.learningrtc.cn.key'),
	cert: fs.readFileSync('./cert/1557605_www.learningrtc.cn.pem')
}

//https server
var https_server = https.createServer(options, app);
var io = socketIo.listen(https_server);
*/

var io = socketIo.listen(http_server);

io.sockets.on('connection', (socket)=> {

	socket.on('message', (room, data)=>{
		socket.to(room).emit('message',room, data);
	});

	socket.on('join', (room)=>{
		socket.join(room);
		var myRoom = io.sockets.adapter.rooms[room]; 
		var users = (myRoom)? Object.keys(myRoom.sockets).length : 0;
		logger.debug('the user number of room is: ' + users);

		if(users < USERCOUNT){
			socket.emit('joined', room, socket.id); 
			if(users > 1){
				socket.to(room).emit('otherjoin', room, socket.id);
			}
		
		}else{
			socket.leave(room);	
			socket.emit('full', room, socket.id);
		}
		
	});

	socket.on('leave', (room)=>{
		// 从管理列表中将用户删除 
		socket.leave(room); 
		var myRoom = io.sockets.adapter.rooms[room]; 
		var users = (myRoom)? Object.keys(myRoom.sockets).length : 0;
		logger.debug('the user number of room is: ' + users);
		// 通知其他用户有人离开了 
		socket.to(room).emit('bye', room, socket.id);
		 // 通知用户服务器已处理 
		socket.emit('leaved', room, socket.id);
	});

});

//https_server.listen(443, '0.0.0.0');




