// Using Krueger-Westermann rendering encodes the position of the vertex as its color
// We use this to render two framebuffers containing the intersection of a
// ray entering and exiting a bounding box respectively

#version 150

in vec3 color;
out vec4 out_color;

void main() {
  out_color = vec4(color, 1.0);
}
