#version 330

precision mediump float;

const int MODE_COLOR = 1;
const int MODE_TEX = 2;
const int MODE_RIG = 3;

//Rendering mode
uniform int u_Mode;

//Interpolated texture coordinate
in vec2 v_TexCoord;

//Texture sampler
uniform sampler2D u_Texture;

//Input color
uniform vec4 u_Color;

in vec4 weightColor;

out vec4 color;

void main(){
	if (u_Mode < 0){
		color = u_Color;
	}
	else if (u_Mode == 0){
		 color = u_Color * texture(u_Texture, v_TexCoord);
	}
	else{
		color = u_Color * texture(u_Texture, v_TexCoord);
      //color = weightColor;
	}
/*
	if (u_Mode == MODE_COLOR){
		color = u_Color;
	}
	else if (u_Mode == MODE_TEX){
		color = u_Color * texture(u_Texture, v_TexCoord);
	}//note that these two branches can be combined
	else{
		color = u_Color * texture(u_Texture, v_TexCoord);
//		color = weightColor;

	}
*/
	//Apple specific, as glAlphaFunc and GL_ALPHA_TEST are deprecated :(
	if (color.a < 0.78) discard;
}
