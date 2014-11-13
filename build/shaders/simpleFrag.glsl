#version 130

precision mediump float;

const int MODE_COLOR = 1;
const int MODE_TEX = 2;
const int MODE_RIG = 3;

//Rendering mode
uniform int u_Mode;

//Interpolated texture coordinate
varying vec2 v_TexCoord;

//Texture sampler
uniform sampler2D u_Texture;

//Input color
uniform vec4 u_Color;

varying vec4 weightColor;

void main(){
	if (u_Mode < 0){
		gl_FragColor = u_Color;
	}
	else if (u_Mode == 0){
		 gl_FragColor = u_Color * texture2D(u_Texture, v_TexCoord);
	}
	else{
		gl_FragColor = u_Color * texture2D(u_Texture, v_TexCoord);
      //gl_FragColor = weightColor;
	}
/*
	if (u_Mode == MODE_COLOR){
		gl_FragColor = u_Color;
	}
	else if (u_Mode == MODE_TEX){
		gl_FragColor = u_Color * texture2D(u_Texture, v_TexCoord);
	}
	else{
		gl_FragColor = u_Color * texture2D(u_Texture, v_TexCoord);
//		gl_FragColor = weightColor;

	}
*/
}
