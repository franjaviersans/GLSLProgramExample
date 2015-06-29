#version 450 core
 
// The uniform paramters which is passed from application for every frame.
uniform float radius;
 
//Store into a texture
layout(binding = 0, rgba16f) uniform writeonly image2D destTex;
 
 
// Declare what size is the group. In our case is 8x8, which gives
// 64 group size.
layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
 
// Declare main program function which is executed once
// glDispatchCompute is called from the application.
void main()
{
    // Read current global position for this thread
    ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
 
    // Calculate the global number of threads (size) for this
    uint gWidth = gl_WorkGroupSize.x * gl_NumWorkGroups.x;
    uint gHeight = gl_WorkGroupSize.y * gl_NumWorkGroups.y;
    uint gSize = gWidth * gHeight;

    ivec2 center = ivec2(gWidth/2, gHeight/2);

    float dist = distance(center, storePos); 

    // Since we have 1D array we need to calculate offset.
    uint offset = storePos.y * gWidth + storePos.x;
 
    vec4 color;

    if(dist < radius) color = mix(vec4(1.0f, 1.0f, 0.0f,1.0f), vec4(0.0f, 1.0f, 1.0f,1.0f), dist/float(radius));
    else color = vec4(0.5f, 0.0f, 0.5f, 1.0f);


    // Store image
    imageStore(destTex, storePos, color);
}