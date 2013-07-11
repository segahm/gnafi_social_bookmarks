var Edit = {
	curEl: null,
	http_request: null,
	editFields: null,
	temp: {syncid: {}},
	messageActive: false,
	confirmDel: function(el,urlid){
		this.clearBack();
		var p = el.parentNode;
		var t = document.createElement('p');
		t.className = "bk-edit-temp";
		this.curEl = t;
		setInnerHTML(t,'Are you sure you want to delete this link?&nbsp;&nbsp;'
			+'<a href="http://" onclick="Edit.finalConfirmDel(this,\''+urlid+'\');return false;">yes</a>&nbsp;&nbsp;'
			+'<a href="http://" onclick="Edit.clearBack();return false;">no</a>');
		p.insertBefore(t,el.nextSibling);
	},
	displayChangesMade: function(str,doclear){
		var e = document.getElementById('edit-link-message');
		if (!this.messageActive){
			setInnerHTML(e,((typeof str) != 'undefined')?str:'changes made..');
			setTimeout('Edit.displayChangesMade(null,true)',3000);
			this.messageActive = true;
		}else if((typeof doclear) != 'undefined'){
			setInnerHTML(e,'');
			this.messageActive = false;
		}
	},
	addNew: function(){
		this.clearBack();
		var p = document.getElementById('edit-link-history');
		p = p.appendChild(document.createElement('div'));
		this.curEl = p;
		setInnerHTML(p,'<form id="edit-link-form"><table><tr valign="top"><td><div class="div-fields">Link: <input type="text" size="60" maxlength="255" name="link" value="'
			+'"/></div><div class="div-fields">Title: <input type="text" size="60" maxlength="100" name="title" value="'
			+'"/></div><div class="div-fields">Keywords: <input type="text" size="50" maxlength="255" name="tags" value="'
			+'"/> <a href="/doc/faq#keywords" class="sideLinks">[tags - ?]</a></div></td><td>Description:<br/>'
			+'<textarea cols="40" rows="3" name="description"></textarea></td></tr></table><input type="submit" onclick="javascript:Edit.saveEdit();" style="display:none;"/></form>'
			+'<span class="bk-edit-delete" style="margin-left:100px;"><a href="javascript:Edit.saveEdit();">[save new]</a>'
			+'&nbsp;&nbsp;&nbsp;<a href="javascript:Edit.clearBack();">[cancel]</a></span>');
	},
	edit: function(el,urlid,syncid){
		this.clearBack();
		this.editFields = {};
		var p = document.getElementById('edit-link-history');
		p = p.appendChild(document.createElement('div'));
		this.curEl = p;
		var ahref = document.getElementById('history-link-'+urlid);
		var link = this.editFields.link = ahref.href;
		var title = this.editFields.title = ahref.firstChild.nodeValue;
		var tags = [];
		this.editFields.tags = '';
		var el;
		for(var i=0;el = document.getElementById('history-link-'+urlid+'-'+i);i++){
			tags.push(el.firstChild.nodeValue);
			this.editFields.tags += tags[i]+' ';
		}
		this.editFields.tags = this.editFields.tags.replace(/[ ]$/,'');
		var descr = '';
		if (el = document.getElementById('history-link-desc-'+urlid)){
			descr = el.firstChild.nodeValue;
		}
		this.editFields.description = descr;
		if ((typeof this.temp.syncid[urlid]) != 'undefined')
			syncid = 1;
		if (document.getElementById('link-pv-'+urlid).value == '1')
			var isprivate = true;
		else
			var isprivate = false;
		setInnerHTML(p,'<form id="edit-link-form"><table><tr valign="top"><td><div class="div-fields">Link: <input type="text" size="60" maxlength="255" name="link" value="'
			+link.unescHtml()+'"/></div><div class="div-fields">Title: <input type="text" size="60" maxlength="100" name="title" value="'
			+title.unescHtml()+'"/></div><div class="div-fields">Keywords: <input type="text" size="50" maxlength="255" name="tags" value="'
			+tags.join(' ')+'"/> <a href="/doc/faq#keywords" class="sideLinks">[tags - ?]</a></div></td><td>Description:<br/>'
			+'<textarea cols="40" rows="3" name="description">'+descr.unescHtml()+'</textarea></td></tr></table><input type="submit" onclick="javascript:Edit.saveEdit('+urlid+');" style="display:none;"/></form>'
			+'<span class="bk-edit-delete" style="margin-left:100px;"><a href="javascript:Edit.saveEdit('+urlid+');">[save]</a>'
			+'&nbsp;&nbsp;&nbsp;<a href="javascript:Edit.clearBack();">[cancel]</a>'
			+'&nbsp;&nbsp;&nbsp;<a href="javascript:Edit.'+(!isprivate?'changePrivate('
			+urlid+',true);">[private]</a>':'changePrivate('
			+urlid+',false);">[make public]</a>')
			+((syncid == null)?'&nbsp;&nbsp;&nbsp;<a href="javascript:Edit.moveUrlToSync(this,'+urlid+');" title="include this url when synchronizing bookmarks between browsers">[use in browser]</a>':'')+'</span>');
	},
	changePrivate: function(urlid,priv){
		this.clearBack();
		document.getElementById('link-pv-'+urlid).value = priv?'1':'0';
		var url = location.href.replace(new RegExp('\\?.*'),'')
		+'?act='+(priv?'makeprivate':'makepublic')+'&urlid='+urlid;
		this.sendGetRequest(url);
	},
	moveUrlToSync: function(el,urlid){
		this.temp.syncid[urlid] = 1;
		this.sendGetRequest(location.href.replace(new RegExp('\\?.*'),'')+'?act=movelink&urlid='+urlid);
		this.clearBack();
		this.displayChangesMade();
	},
	saveEdit: function(urlid){
		var isInEditMode = ((typeof urlid) != 'undefined');
		var ur = '';
		if (isInEditMode)url = 'http://gnafi.com/mine?act=edithist&urlid='+urlid;
		else url = 'http://gnafi.com/mine?act=savenew';
		if (!this.initHttpRequest())alert('connection to the server failed');
		var frm = document.getElementById("edit-link-form");
		var fields = {link: frm['link'].value,title:frm['title'].value,
			description: frm['description'].value,tags: frm['tags'].value};
		if (fields.link.trim() == '' || fields.title.trim() == ''){
			alert('required fields are empty');
			return;
		}
		var str = '';
		for(var i in fields){
			fields[i] = fields[i].trim();
			if (!isInEditMode || (fields[i] != this.editFields[i]))
				str += '&'+i+'='+encodeURIComponent(fields[i]);
		}
		//don't make a request if nothing was changed
		if (str != ''){
			this.http_request.open('POST', url, true);
			if (!isInEditMode){
				this.http_request.onreadystatechange = function(){
					if (Edit.http_request.readyState == 4
							&& Edit.http_request.status == 200) 
					window.location.reload();
				}
			}
			this.http_request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
			this.http_request.send(str);
			if (isInEditMode){
				//change text on the page
				var link, descr;
				var li = document.getElementById('link-'+urlid);
				link = document.getElementById('history-link-'+urlid);
				if (descr = document.getElementById('history-link-desc-'+urlid)){
					descr.replaceChild(document.createTextNode(fields.description),descr.firstChild);
				}else if(fields.description != ''){
					descr = document.createElement('p');
					descr.setAttribute('id','history-link-desc-'+urlid);
					descr.className = 'description';
					descr.appendChild(document.createTextNode(fields.description));
					li.appendChild(descr);
				}	
				link.setAttribute('href',fields.link);
				link.replaceChild(document.createTextNode(fields.title),link.firstChild);
				var tags = fields.tags.split(/[ ]+/);
				var i;
				for(i=0;el = document.getElementById('history-link-'+urlid+'-'+i);i++){
					el.parentNode.removeChild(el);
				}
				for(i=0;i<tags.length;i++){
					var a = document.createElement('a');
					a.setAttribute('id','history-link-'+urlid+'-'+i);
					a.className = 'tag';
					a.setAttribute('href','/p/'+encodeURIComponent(tags[i]));
					a.appendChild(document.createTextNode(tags[i]))
					if (descr)
						li.insertBefore(a,descr);
					else
						li.appendChild(a);
				}
				this.displayChangesMade();
			}
		}
		this.clearBack();
	},
	finalConfirmDel: function(el,urlid){
		if (this.curEl != null){
			var obj = this.curEl;
			while (!obj.nodeName.match(/li/i)){
				obj = obj.parentNode;
			}
			obj.parentNode.removeChild(obj);
			this.curEl = null;
		}
		var url = location.href.replace(new RegExp('\\?.*'),'')+'?act=deletehist&urlid='+urlid;
		this.sendGetRequest(url);
		this.displayChangesMade();
	},	
	clearBack: function(el,urlid){
		if (this.curEl != null){
			var p = this.curEl.parentNode;
			p.removeChild(this.curEl);
			this.curEl = null;
		}
	},
	sendGetRequest: function(url){
		var head = document.getElementsByTagName('head')[0];
		var sc = document.createElement('script');
		sc.src = url;
		sc.type = 'text/javascript';
		head.appendChild(sc);
	},
	initHttpRequest: function(){
		if (this.http_request != null)return true;
		if (window.XMLHttpRequest) { // Mozilla, Safari,...
			this.http_request = new XMLHttpRequest();
	        if (this.http_request.overrideMimeType) {
	      		this.http_request.overrideMimeType('text/html');
	        }
		} else if (window.ActiveXObject) { // IE
			try {
				this.http_request = new ActiveXObject("Msxml2.XMLHTTP");
			} catch (e) {
				try {
					this.http_request = new ActiveXObject("Microsoft.XMLHTTP");
				} catch (e) {}
			}
		}
		if (!this.http_request) {
			return false;
		}
		return true;
	}
}