// MyColor object
function MyColor()
{
 this.clr = null;
 this.red = color_red;
 this.green = color_green;
 this.blue = color_blue;
 this.ToString = color_tostring;
 this.FromString = color_fromstring;
 this.rgb = color_rgb;
}

function color_blue()
{
 return (this.clr & 0x000000FF);
}

function color_green()
{
 var res = this.clr & 0x0000FF00;
 res = res >>> 8;
 return res;
}
function color_red()
{
 var res = this.clr & 0x00FF0000;
 res = res >>> 16;
 return res;
}
function color_tostring()
{
 var s = "#";
 var r = this.red().toString(16);
 var g = this.green().toString(16);
 var b =  this.blue().toString(16);

 if(r.length == 0)
  r = "00";
 if(r.length == 1)
  r = "0" + r;
 if(g.length == 0)
  g = "00";
 if(g.length == 1)
  g = "0" + g;  	
 if(b.length == 0)
  b = "00";
 if(b.length == 1)
  b = "0" + b; 	
 return s + r + g + b;
}
// from "#rrggbb"
function color_fromstring( s )
{
 if(typeof(s) == "string"){
  if(s.charAt(0) == '#'){
	 s = s.slice(1);
	 s = "0x" + s;
	 var clr = parseInt(s);
	 var r = clr & 0x00FF0000;
	 var g = clr & 0x0000ff00;
	 var b = clr & 0x000000FF;
	 this.clr = (r | g | b );
	}
 }
}
function color_rgb( r, g, b)
{
 this.clr = (( r << 16 ) | ( g << 8 ) | b);
}

// Lighter object 

function Lighter()
{
 this.count = 2;
 this.clrOld = new MyColor;
 this.clrNew = new MyColor;
 this.Light = lighter_light;
}

function lighter_light( bInc , inc )
{
	var r = this.clrOld.red();
	var g = this.clrOld.green();
	var b = this.clrOld.blue();

	if(bInc){
	  if(r != 0)
		 r += inc;
		if(g != 0)
		 g += inc;
		if(b != 0)
		 b += inc;
	}
	else{
	  r -= inc;
		g -= inc;
		b -= inc;
	}
	r = Math.max(0,Math.min(r,255));
	g = Math.max(0,Math.min(g,255));
	b = Math.max(0,Math.min(b,255));

	this.clrNew.rgb(r,g,b);
}
