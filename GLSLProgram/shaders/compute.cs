#version 450 core
 
//Store into a texture
layout(binding = 0, rgba16f) uniform writeonly image2D destTex;
 
#define MAX_ITERATIONS 100
 
 
// Declare what size is the group. In our case is 8x8, which gives
// 64 group size.
layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

vec4 CompWindow = vec4(-2.0f, -2.0f, 2.0f, 2.0f); 

uint mandelbrot( vec2 c )
{
    vec2 z = vec2(0.0f, 0.0f);
    uint i = 0;
    while(i< MAX_ITERATIONS && (z.x * z.x + z.y *z.y) < 4.0)
    {
        z = vec2(z.x * z.x - z.y * z.y + c.x, 2 * z.x * z.y + c.y);
        ++i;
    }
    return i;
}


// Declare main program function which is executed once
// glDispatchCompute is called from the application.
void main()
{
    // Read current global position for this thread
    ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
 
    // Calculate the global number of threads (size) for this
    ivec2 size = imageSize(destTex);
    if(storePos.x < size.x && storePos.y < size.y)
    {
        float dx = (CompWindow.z - CompWindow.x) / size.x;
        float dy = (CompWindow.w - CompWindow.y) / size.y;        

        vec2 c = vec2(
                dx * gl_GlobalInvocationID.x + CompWindow.x,
                dy * gl_GlobalInvocationID.y + CompWindow.y
                );
 

        vec4 color = vec4(0.0f, 0.5f, 0.5f, 1.0f);
        uint i = mandelbrot(c);
        if(i < MAX_ITERATIONS)
        {
            if(i < 5) color = vec4(float(i)/5.0f, 0.0f,0.0f,1.0f);
            else if(i < 10) color = vec4((float(i) - 5.0f) / 5.0f, 1.0f,0.0f,1.0f); 
            else if(i < 15) color = vec4(1.0f, 0.0f, (float(i) - 10.0f) / 5.0f, 1.0f);
            else color = vec4(0.0f,0.0f,1.0f,0.0f); 
        }
        else color = vec4(0.0f, 0.0f, 0.0f, 1.0f);

        // Store image
        imageStore(destTex, storePos, color);
    }
}