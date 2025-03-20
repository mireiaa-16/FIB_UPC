#version 330 core

out vec4 FragColor;
in vec4 vertex_color;

uniform int esBase;

void main() {
    if (esBase == 1) {
        ivec2 pos = ivec2(gl_FragCoord.xy);

        int mida = 5;

        bool forat = (pos.x % (2 * mida) < mida) &&
                    (pos.y % (2 * mida) < mida);
        
        if (forat) discard;
        else FragColor = vertex_color;
    }
    else {
        FragColor = vertex_color;
    }
}

