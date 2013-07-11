var URL = {
	path: function(url){
		//convert "http://domain/path" into "/path"
		if (url.match(new RegExp('^[^\/]+\:\/\/.*','gi')))
			url = url.replace(new RegExp('^[^\/]+:\/\/[^\/]+'),"");
		url = url.replace(new RegExp('[\/]{2,}'),"/")
		//if we got an invalid path (ex: empty), fix it
		if (url.charAt(0) != '/')url = "/"+url;
		if ((idx = url.indexOf("?")) != -1)url = url.substr(0,idx);
		//strip slash at the end
		if (url.charAt(url.length-1) == "/" && url.length != 1)
			url = url.substr(0,url.length-1);
		return url;
	},
	root: function(url){
		url = this.path(url);
		return url.replace(new RegExp('[^\/]+$',''),'');
	},
	tag: function(url){
		return this.path(url).replace(new RegExp('^.*\/','g'),'')
			.replace(new RegExp('[_+]+','g'),' ').replace(new RegExp('[^a-z &].*','g'),'');
	},
	tree: function(path){
		path = path.replace(new RegExp('[_+]+','g'),' ').replace(new RegExp('[/]{2,}','g'),'/');
		var ar = path.split('/');
		if (ar[0] == '')ar.shift();
		if (ar[ar.length-1] == '')ar.pop();
		return ar;
	},
	constructPath: function(root,word){
		word = word.replace(new RegExp('[^\&a-z \/]','gi'),' ').replace(new RegExp('[ ]*/[ ]*'),'/').trim()
			.replace(new RegExp('[ ]+','g'),'+');
		return this.path(root+word);
	}
}
function Crumb(divid,urladdon){
	this.divid = divid;
	this.hrefLink = null;
	this.go = function(root,word){
		this.root = root;
		ObjectStorage.set("Crumb-"+divid,this);
		if ((typeof urladdon) == 'undefined' || urladdon == "")
			urladdon = "crmb=1";
		//if special urladdon
		if (urladdon.indexOf('<<p>>') != -1)
			this.hrefLink = urladdon;
		else
			this.hrefLink = '<<p>>?'+urladdon;
		this.hrefLink = this.hrefLink.strReplace('??','?');
		var div = getObjectByID(divid);
		var ar = URL.tree(root);
		var str = "";
		var url = "/p/";
		var flag = false;
		for (var i=0;i<ar.length;i++){
			if (flag){
				str += '<span class="'+divid+'-delim">/</span>';
				url += "/";
			}
			flag = true;
			url += ar[i];
			str += '<a href="'+this.hrefLink.strReplace('<<p>>',url)+'" class="'+divid+'-word">'
				+ar[i].replace(/[_+]/," ")+'</a>';
		}
		if (flag)
			str += '<span class="'+divid+'-delim">/</span>';
		str += '<input class="resizableBox" type="text" id="'+divid+'-crumb"'
			+' value="'+URL.tag(word)+'"/>';
		this.tag = URL.tag(word);
		setInnerHTML(div,str);
		var crumbelem = getObjectByID(divid+'-crumb');
		resizeToText(crumbelem,crumbelem.value,20);
		crumbelem.onblur = this.blur; 
		crumbelem.onfocus = this.focus;
		crumbelem.onmouseover = this.mouseover;
		crumbelem.onmouseout = this.mouseout;
		crumbelem.onkeyup = this.keyhandler;
	}
	this.mouseover = function(e) {
		var crumbelem = $eTarget(e);
		crumbelem.style.borderColor = "#000000";
	}
	this.mouseout = function(e) {
		var crumbelem = $eTarget(e);
		crumbelem.style.borderColor = "#999999";
	}
	this.focus = function(e) {
		var crumbelem = $eTarget(e);
		crumbelem.style.color = "#000000";
		crumbelem.style.fontWeight = "bold";
	}
	this.blur = function(e) {
		var o = ObjectStorage.get("Crumb-"+divid);
		var crumbelem = $eTarget(e);
		crumbelem.style.color = "#999999";
		crumbelem.style.fontWeight = "normal";
		var l = crumbelem.value.length;
		crumbelem.value = o.tag;	//remind user to press enter
		if (l != crumbelem.value.length)
			resizeToText(crumbelem,crumbelem.value,20);
	}
	this.keyhandler = function(e) {
		var crumbelem = $eTarget(e);
		e = $e(e);
		if ($key(e) == 13) {
			var crumb = ObjectStorage.get("Crumb-"+divid);
			crumb.tag = crumbelem.value;
			var tag = URL.constructPath(crumb.root,crumbelem.value);
			if (tag) {
				var o = ObjectStorage.get("Crumb-"+divid);
				if (tag.indexOf('?') != -1)
					o.hrefLink = o.hrefLink.strReplace('?','&');
				location.href = o.hrefLink.strReplace('<<p>>','/p'+tag);
			}
		}else{
			resizeToText(crumbelem,crumbelem.value,20);
		}
	}
}