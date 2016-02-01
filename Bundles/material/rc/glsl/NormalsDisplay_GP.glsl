#version 150

layout(triangles) in;

#    ifdef POINT
layout(line_strip, max_vertices = 6) out;

#    else // CELL
layout(line_strip, max_vertices = 2) out;
#    endif

uniform mat4 u_worldViewProj;
uniform float u_normalLength;

#    ifdef POINT
in VertexDataIn
{
    vec3 normalVP;
} vertexIn[];
#    endif

#    ifndef DEPTH
uniform vec4 u_normalColor;
out VertexDataOut
{
    vec4 oColor;
} vertexOut;

#    endif

void main()
{
#    ifdef POINT
        for(int i = 0; i < 3; i++)
        {
            vec4 P = gl_in[i].gl_Position;
            vec3 N = vertexIn[i].normalVP;

            // Vertex normal origin
            gl_Position = u_worldViewProj * P;

#        ifndef DEPTH
            vertexOut.oColor = u_normalColor;
#        endif

            EmitVertex();

            // Vertex normal summit
            gl_Position = u_worldViewProj * (P + vec4(N, 0.) * u_normalLength);

#        ifndef DEPTH
            vertexOut.oColor = u_normalColor;
#        endif

            EmitVertex();
            EndPrimitive();
        }

#    else // CELL
        vec4 P0 = gl_in[0].gl_Position;
        vec4 P1 = gl_in[1].gl_Position;
        vec4 P2 = gl_in[2].gl_Position;

        vec4 V0 = P0 - P1; // P1->P0
        vec4 V1 = P2 - P1; // P1->P2

        // Face normal, which is the cross product of V0 and V1
        vec3 N = cross(V1.xyz, V0.xyz);
        N = normalize(N);

        // Center of the triangle
        vec4 P = (P0 + P1 + P2) / 3.0;

        // Cell normal origin
        gl_Position = u_worldViewProj * P;

#        ifndef DEPTH
            vertexOut.oColor = u_normalColor;
#        endif

        EmitVertex();

        // Cell normal summit
        gl_Position = u_worldViewProj * (P + vec4(N, 0.) * u_normalLength);

#        ifndef DEPTH
            vertexOut.oColor = u_normalColor;
#        endif

        EmitVertex();
        EndPrimitive();
#    endif
}
