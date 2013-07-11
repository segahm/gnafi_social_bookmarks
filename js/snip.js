var Snip = {
	tagsLoad: function(){
		var d = document.getElementById('link-out');
		setInnerHTML(d,Snip.gnafi);
	},
	doLiveMark: function(){
		var l = document.getElementById('bookmark-link');
		var p = document.getElementById('bookmark-path');
		var t = document.getElementById('bookmark-title');
		if (l.value.trim() == '' || t.value.trim() == '')return;
		var li = l.value.trim();
		var path = p.value.trim();
		var title = t.value.trim();
		if (path == '')path = '/';
		var url = 'livefeed:path='+encodeURIComponent(path)+'&url='
			+encodeURIComponent(li);
		var styles = document.getElementById('bookmark-style');
		var str = '<a href="'+url+'" style="'+styles.value+'" id="gnafi-feed-link">'
			+title+'</a>\n<script type="text/javascript">\n<!--\nvar o = '
			+'document.getElementById("gnafi-feed-link");if (!o.pathname)o.'
			+'setAttribute("onclick","window.location = \'http://gnafi.com/download.xpi\';'
			+'return false;");\n//-->'
			+'\n</s'+'cript>';
		document.getElementById('text-out').value = str;
		setInnerHTML(document.getElementById('link-out'),str);
	},
	doTag: function(){
		var div = document.createElement('div');
		var t = document.getElementById('snip-tag');
		t = t.value;
		var o = document.getElementById('text-out');
		var d = document.getElementById('link-out');
		o.value = '';
		setInnerHTML(d,'');
		if (t.trim() == ''){
			return;	
		}
		var url = 'http://gnafi.com/snip/p'+URL.constructPath('',t)+'?';
		var form = document.getElementById('tag-form');
		var r = new RegExp('^j_');
		var r2 = new RegExp('^checkbox$','i');
		for(var i in form){
			if (i.match(r)){
				if (form[i].type.match(r2)){
					if (form[i].checked)
						url += '&'+i.substr(2)+'='+encodeURIComponent(form[i].value);
				}else{
					url += '&'+i.substr(2)+'='+encodeURIComponent(form[i].value);
				}
			}
		}
		var url1 = url+'&inout=docwrite';
		o.value = '<script type="text/javascript" src="'+url1+'"></s'+'cript>';
		//force load on the file
		var e = document.getElementsByTagName('head');
		if (e.length == 0)
			e = document.getElementsByTagName('body');
		var scr = document.createElement('script');
		url += '&inout=Snip.gnafi&callback=Snip.tagsLoad';
		scr.src = url;
		scr.id = 'gnafi-snip-script';
		scr.type = "text/javascript";
		e[0].appendChild(scr);
	}
}