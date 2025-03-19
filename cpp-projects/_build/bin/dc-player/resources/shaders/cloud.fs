#version 430 core

// layout
layout( location = 0 ) out vec4 FragColor;

// varyings
// # in
in vec4 vFragColorVs;
flat in int discardVertex;

void main(){

	if(discardVertex == 1){
		discard;
	}

	FragColor = vFragColorVs;    
}
