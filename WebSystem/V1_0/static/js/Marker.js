var map = new AMap.Map("container", {
	resizeEnable: true,
	center: [116.481181, 39.989792],
	zoom: 16
});

//添加marker标记
function addMarker(x,y) {
	var marker = new AMap.Marker({
								 map: map,
								 position: [x,y]
	});
	//鼠标点击marker弹出自定义的信息窗体
	AMap.event.addListener(marker, 'click', function () {
		$(document).ready(function(){
			$.getJSON('Marker',{'Lng':marker.getPosition().getLng(),'Lat':marker.getPosition().getLat()},function(ret){
				var obj=jQuery.parseJSON(ret);
				var title = obj.Name+'<p style="font-size:11px;color:#F00;">价格:'+obj.Price+'</p>';
				var content=[];
				//content.push("<img src='http://tpc.googlesyndication.com/simgad/5843493769827749134'>地址:"+obj.Address);
				content.push("地址："+obj.Address);
				content.push("电话："+obj.Phone);
				content.push("剩余车位/总车位："+obj.F_TS+"/"+obj.A_TS);
				content.push("是否支持预约："+obj.Subscribe_Permit);
				content.push("数据最后更新时间："+obj.Motify_Time);
				var infoWindow = new AMap.InfoWindow({
													 isCustom: true,  //使用自定义窗体
													 content: createInfoWindow(title, content.join("<br/>")),
													 offset: new AMap.Pixel(16, -45)
				});
				infoWindow.open(map, marker.getPosition());
			});
		});
		//infoWindow.open(map, marker.getPosition());
	});
}

//构建自定义信息窗体
function createInfoWindow(title, content) {
	var info = document.createElement("div");
	info.className = "custom-info input-card content-window-card";

	//可以通过下面的方式修改自定义窗体的宽高
	//info.style.width = "400px";
	// 定义顶部标题
	var top = document.createElement("div");
	var titleD = document.createElement("div");
	var closeX = document.createElement("img");
	top.className = "info-top";
	titleD.innerHTML = title;
	closeX.src = "https://webapi.amap.com/images/close2.gif";
	closeX.onclick = closeInfoWindow;

	top.appendChild(titleD);
	top.appendChild(closeX);
	info.appendChild(top);

	// 定义中部内容
	var middle = document.createElement("div");
	middle.className = "info-middle";
	middle.style.backgroundColor = 'white';
	middle.innerHTML = content;
	info.appendChild(middle);

	// 定义底部内容
	var bottom = document.createElement("div");
	bottom.className = "info-bottom";
	bottom.style.position = 'relative';
	bottom.style.top = '0px';
	bottom.style.margin = '0 auto';
	var sharp = document.createElement("img");
	sharp.src = "https://webapi.amap.com/images/sharp.png";
	bottom.appendChild(sharp);
	info.appendChild(bottom);
	return info;
}

//关闭信息窗体
function closeInfoWindow() {
	map.clearInfoWindow();
}
