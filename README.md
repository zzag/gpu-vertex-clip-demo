# GPU based geometry clipping

This example demonstrates that the rendered geometry does not necessarily have to 
be clipped on the CPU. It can be done on the GPU using instanced rendering and the
`gl_ClipDistance`.

```glsl
#version 330\n

layout (location = 0) in vec2 position;
layout (location = 1) in vec4 clipRect;

uniform mat4 projectionMatrix;

void main()
{
    gl_ClipDistance[0] = position.x - clipRect[0];
    gl_ClipDistance[1] = clipRect[1] - position.x;
    gl_ClipDistance[2] = position.y - clipRect[2];
    gl_ClipDistance[3] = clipRect[3] - position.y;

    gl_Position = projectionMatrix * vec4(position, 0.0, 1.0);
}
```
