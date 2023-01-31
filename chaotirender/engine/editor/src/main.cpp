#include <iostream>

#include <runtime/render_pipeline.h>

#include <runtime/test.h>

int main(int argc, char** argv) 
{   
    // TODO: 
    // 1. option flag: eraly-z, back face culling
    // 2. shader define variables: atrribute, varying, uniform
    // 3. does renderPipeline need to be global?
    // 4. Texture class: w, h, memory, get(u, v)
    // Chaotirender::testPipeline();
    Chaotirender::runPipeline();

    // Chaotirender::testInterpolation();

    return 0;
}