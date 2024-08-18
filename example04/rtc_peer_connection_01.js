/*
RTCPeerConnection
创建RTCPeerConnection时,需要传递参数, 通过参数可以对数据传输方式做一些策略选择, 比如 中继方式 还是p2p ,使用哪些中继服务器
通过getUserMedia采集音视频数据 通过RTCPeerConnection发送给对方

将采集到的音视频数据绑定到RTCPeerConnection,RTCPeerConnection提供两个函数 {
    addStream() 标记为过时
    addTrack()
}

当RTCPeerConnection对象与音视频绑定后,就需要进行媒体协商
当媒体协商完成以后, webrtc 就开始建立网络连接了,其过程称为ICE
更确切地说，ICE是在各端调用setLocalDescription（）接
口后就开始了。其操作过程如下：收集Candidate[2]，交换
Candidate，按优先级尝试连接

Candidate正是WebRTC用来描述它可以连接的远端的基本信息，因此
它是至少包括{address，port，protocol}三元组的一个信息集。例如：
IceCandidate{ 
    "candidate": "udp 192.168.1.9 45845 type host … ufrag aOj8 … ", 
    "sdpMid":"0", 
    "sdpMLineIndex":0 
}
 
从上述得知信息： Candidate类型（type host）  用户名（ufrag a0j8）
Candidate分成了四种类型，即host srflx prflx relay

收集Candidate
WebRTC收集Candidate时有几种途径：host类型的Candidate，是
根据主机的网卡个数来决定的，一般来说，一个网卡对应一个IP地
址，给每个IP地址随机分配一个端口从而生成一个host类型的
Candidate；srflx类型的Candidate，是从STUN服务器获得的IP地址和
端口生成的；relay类型的Candidate，是通过TRUN服务器获得的IP地
址和端口号生成的。

收集到Candidate后，为了通知上层，WebRTC还在
RTCPeerConnection对象中提供了一个事件，即onicecandidate。为了
将收集到的Candidate交换给对端，需要为onice candidate事件设置
一个回调函数。

通过信令系统交换Candidate 形成  CandidatePair 然后尝试连接

RTCPeerConnection对象的ontrack（）事件。每当有远端的音视频数
据传过来时，ontrack（）事件就会被触发。因此你只需要给
ontrack（）事件设置一个回调函数，就可以拿到远端的MediaStream
了。

*/

/*

1. 检查媒体支持情况
2. 获取媒体数据
3. 获取房间号，连接信令服务->join
4. joined 加入后 创建 RTCPeerConnection 对象pc
   
    设置好这两个回调

    收集到的Candidate
    pc.onicecandidate => (){
        // 给对端发送Candidate
        sendMessage(roomid, {
            type: 'candidate',
            label:event.candidate.sdpMLineIndex, 
            id:event.candidate.sdpMid, 
            candidate: event.candidate.candidate
        });
    }

    收到对端的媒体数据
    pc.ontrack => () {
        显示出来
    }

5. 在pc对象中 绑定媒体数据 pc.addTrack(track, localStream);

6. 如果收到其他人连接的信息, otherjoin, 这时候状态变为 joined_conn
   
   创建offer,setLocalDescription 向信令服务发送offer

   另一个用户在收到 offer信息后, setRemoteDescription(offer)
   会创建answer setLocalDescription 向信令发送answer

   在收到answer后  setRemoteDescription(answer)
 
7. 收到 对方的candidate 后
    将远端Candidate 消息添加到PeerConnection 中 
    pc.addIceCandidate(candidate);	

    会与本地的Candidate形成
    CandidatePair（即连接候选者对）。有了CandidatePair，WebRTC就
    可以开始尝试建立连接了。

*/