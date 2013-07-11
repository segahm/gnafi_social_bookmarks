/*This file contains common functions (utilities) that are independant of the 
 *site functionality*/

String.prototype.trim = function(){ return this.replace(/^\s+|\s+$/g,'') }
//makes text safe for html view
String.prototype.unescHtml = function(){
	r = this;
	var e = [['<','&lt;'],['>','&gt;'],['&','&amp;'],['"','&quot;']];
	for(var i=0;i<e.length;i++){
		r = r.strReplace(e[i][0],e[i][1])}
	return r;
}
//string replace without using reg exp
String.prototype.strReplace = function(search,replaceWith){
	var pos = 0;
	var l = search.length;
	var str = this;
	while ((pos = this.indexOf(search,pos)) != -1){
		str = str.substr(0,pos)+replaceWith+str.substr(pos+l);
		pos += l;
	}
	return str;
}
//gets rid of all html tags
String.prototype.stripHTML = function(){
	return this.replace(/(<([^>]+)>)/ig,"").replace(/[\r\n ]/g," ").trim();
}		
/*global storage for objects, can be used to overcome problems with scope 
 *like when an event handler becomes part of the event target and not the parent class
 *(i.e. this won't work) 
 */
var ObjectStorage = {
	storage: new Array(),
	set: function(name,value){
		this.storage[name] = value;
	},
	get: function(name){
		return this.storage[name];
	}
}
/*void*/
function myvoid(){}
// get previous/next non-text node
function previousElement(o) {
	if(o.previousSibling) { while (o.previousSibling.nodeType != 1) o = o.previousSibling; return o.previousSibling }
	else return false
}
function nextElement(o) {
	if(o.nextSibling) { while (o.nextSibling.nodeType != 1) o = o.nextSibling; return o.nextSibling }
	else return false
}
//use short (mozilla) types - click instead of onclick
function addEvent(e,type,func){
	if (e.addEventListener){
		e.addEventListener(type,func,false);
	}else if(e.attachEvent){
		e.attachEvent('on'+type,func)
	}
}
// styling functions
function isA(o,klass){ if(!o.className) return false; return new RegExp('\\b'+klass+'\\b').test(o.className) }
function addClass(o,klass){ if(!isA(o,klass)) o.className += ' ' + klass }
function rmClass(o,klass){ o.className = o.className.replace(new RegExp('\\s*\\b'+klass+'\\b'),'') }
function swapClass(o,klass,klass2){ var swap = isA(o,klass) ? [klass,klass2] : [klass2,klass]; rmClass(o,swap[0]); addClass(o,swap[1]) }
function getStyle(o,s) {
	if (document.defaultView && document.defaultView.getComputedStyle) return document.defaultView.getComputedStyle(o,null).getPropertyValue(s)
	else if (o.currentStyle) { return o.currentStyle[s.replace(/-([^-])/g, function(a,b){return b.toUpperCase()})] }
}
// shorter names for grabbing stuff
function $id(id){ return getObjectByID(id)}
function $tags(t,o){ o=o||document; return o.getElementsByTagName(t) }
function $tag(t,o,i) { o=o||document; return o.getElementsByTagName(t)[i||0] }
//event
function $e(e){ if (!e) var e = window.event; return e;}
//element from event
function $eTarget(e){
	var e = $e(e);
	var targ;
	if (e.target) targ = e.target;
	else if (e.srcElement) targ = e.srcElement;
	if (targ.nodeType == 3) // defeat Safari bug
		targ = targ.parentNode;
	return targ;
}
//keycode from event
function $key(e){
	e = $e(e);
	if (e.keyCode) code = e.keyCode;
	else if (e.which) code = e.which;
	return code;
}
// get elements by class name, eg $c('post', document, 'li')
function $c(c,o,t) { o=o||document;
	if (!o.length) o = [o]
	var elements = []
	for(var i = 0, e; e = o[i]; i++) {
		if(e.getElementsByTagName) {
			var children = e.getElementsByTagName(t || '*')
			for (var j = 0, child; child = children[j]; j++) if(isA(child,c)) elements.push(child)
	}}
	return elements
}

// Set the "inside" HTML of an element.
function setInnerHTML(element, toValue)
{
	// IE has this built in...
	if (typeof(element.innerHTML) != 'undefined')
		element.innerHTML = toValue;
	else
	{
		var range = document.createRange();
		range.selectNodeContents(element);
		range.deleteContents();
		element.appendChild(range.createContextualFragment(toValue));
	}
}
//getXY()
 //
 // Returns the position of any element as an object.
 //
 // Typical usage:
 // var pos = getXY(object);
 // alert(pos.x + " " +pos.y);
function getXY(el) {
  var x = el.offsetLeft;
  var y = el.offsetTop;
  if (el.offsetParent != null) {
    var pos = getXY(el.offsetParent);
    x += pos.x;
    y += pos.y;
  }
  return {x: x, y: y}
}
// Set the "outer" HTML of an element.
function setOuterHTML(element, toValue)
{
	if (typeof(element.outerHTML) != 'undefined')
		element.outerHTML = toValue;
	else
	{
		var range = document.createRange();
		range.setStartBefore(element);
		element.parentNode.replaceChild(range.createContextualFragment(toValue), element);
	}
}

// Get the inner HTML of an element.
function getInnerHTML(element)
{
	if (typeof(element.innerHTML) != 'undefined')
		return element.innerHTML;
	else
	{
		var returnStr = '';
		for (var i = 0; i < element.childNodes.length; i++)
			returnStr += getOuterHTML(element.childNodes[i]);

		return returnStr;
	}
}

function getOuterHTML(node)
{
	if (typeof(node.outerHTML) != 'undefined')
		return node.outerHTML;

	var str = '';

	switch (node.nodeType)
	{
		// An element.
		case 1:
			str += '<' + node.nodeName;

			for (var i = 0; i < node.attributes.length; i++)
			{
				if (node.attributes[i].nodeValue != null)
					str += ' ' + node.attributes[i].nodeName + '="' + node.attributes[i].nodeValue + '"';
			}

			if (node.childNodes.length == 0 && in_array(node.nodeName.toLowerCase(), ['hr', 'input', 'img', 'link', 'meta', 'br']))
				str += ' />';
			else
				str += '>' + getInnerHTML(node) + '</' + node.nodeName + '>';
			break;

		// 2 is an attribute.

		// Just some text..
		case 3:
			str += node.nodeValue;
			break;

		// A CDATA section.
		case 4:
			str += '<![CDATA' + '[' + node.nodeValue + ']' + ']>';
			break;

		// Entity reference..
		case 5:
			str += '&' + node.nodeName + ';';
			break;

		// 6 is an actual entity, 7 is a PI.

		// Comment.
		case 8:
			str += '<!--' + node.nodeValue + '-->';
			break;
	}

	return str;
}
//makes sure that str is smaller than l and then appends "..." to the end
function cutLength(str,l){
	if (str != null && str.length > l) str = str.substr(0,l-3)+'...';
	return str;
}
function getObjectByID(id,o) {
 var c,el,els,f,m,n; if(!o)o=document; if(o.getElementById) el=o.getElementById(id);
 else if(o.layers) c=o.layers; else if(o.all) el=o.all[id]; if(el) return el;
 if(o.id==id || o.name==id) return o; if(o.childNodes) c=o.childNodes; if(c)
 for(n=0; n<c.length; n++) { el=getObjectByID(id,c[n]); if(el) return el; }
 f=o.forms; if(f) for(n=0; n<f.length; n++) { els=f[n].elements;
 for(m=0; m<els.length; m++){ el=getObjectByID(id,els[n]); if(el) return el; } }
 return null;
}
function hideFunc(e, show) { return (function(){show.style.display = 'inline'; e.style.display = 'none'; this.style.display = 'none'}) }
function showFunc(e, hide) { return (function(){hide.style.display = 'inline'; e.style.display = 'block'; this.style.display = 'none'}) }

function resizeToText(o, text, margin) {
	margin = margin || 0;
	var c = getObjectByID(o.id + '-copy');
	if (!c) { makeResizeThing(o); c = getObjectByID(o.id + '-copy') };
	//escape html for div element
	var esc = {'<':'[','>':']',' ':'&nbsp;'};
	for(var i in esc) text=text.replace(new RegExp(i,'g'), esc[i]);
	setInnerHTML(c,text);
	//fix the IE bug with offsetWidth initilization
	if (text.length != 0 && c.offsetWidth == 0)
		o.style.width = Math.max(1,text.length*0.75) + 'em';
	else
		o.style.width = c.offsetWidth + margin + 'px';
}
function makeResizeThing(src) {
	var o = document.createElement('div');
	o.style.position = 'absolute'; o.style.top = o.style.left = 0;
	o.style.visibility = 'hidden';
	o.style.fontSize = getStyle(src, 'font-size');
	o.style.fontFamily = getStyle(src, 'font-family');
	o.id = src.id + '-copy';
	src.parentNode.appendChild(o);
}