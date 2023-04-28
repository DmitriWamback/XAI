#if defined(__APPLE__)
    #include <OpenAL/al.h>
    #include <OpenCL/cl.h>
#else
    #include <AL/al.h>
#endif

#include <iostream>
#include <string>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace Xai::Audio {

    const int BufferSamples = 8192;
    const int NumberBuffers = 4;

    class Source {
    public:
        uint32_t soundbuf;
        uint32_t buffers[NumberBuffers];
        static Source* Create(const char* filename);
        void Play();
        void Stop();
    };

    Source* Source::Create(const char* filename) {
        
        Source* source = new Source();
        alGenSources(1, &source->soundbuf);
        alGenBuffers(NumberBuffers, source->buffers);

        size_t frame;

        return source;
    };
    void Source::Play() {
        
    }
    void Source::Stop() {

    }


    class AudioPlayer {
    public:
        std::map<std::string, Xai::Audio::Source> sources;
    };
}

void XAI_COMPUTATION_LOG(cl_int err, std::string message) {
    
    if (err != CL_SUCCESS) std::cout << message << '\n';
}

void XAI_COMPUTATION_LOG(void* asset, std::string message) {

    if (asset == NULL || !asset) std::cout << message << '\n';
}

namespace Xai::Computation {

    typedef struct ComputeKernel {
        cl_context          context;
        cl_device_id        id;
        cl_command_queue    cmdqueue;
        cl_program          program;
        cl_kernel           kernel;
        size_t              local, global;
        cl_mem input, output;

        void Compute(const char* filepath, const char* mainFunctionName) {

            int count = 1000000;
            float data[count], results[count];

            XAI_COMPUTATION_LOG(clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, &id, NULL), "Failed to retrieve ids");

            FILE* fileptr = fopen(filepath, "r");
            int strsize, readsize;
            char* buffer = NULL;
            XAI_COMPUTATION_LOG(fileptr, "XAI::COMPUTATION: Couldn't open the file");

            fseek(fileptr, 0, SEEK_END);
            strsize = ftell(fileptr);
            rewind(fileptr);
            buffer = (char*)malloc(sizeof(char) * (strsize+1));
            readsize = fread(buffer, sizeof(char), strsize, fileptr);
            buffer[strsize] = '\0';
            if (strsize != readsize) free(buffer);
            fclose(fileptr);

            cl_int errc;
            context = clCreateContext(0, 1, &id, NULL, NULL, &errc);
            XAI_COMPUTATION_LOG(context, "XAI::COMPUTATION: Failed to create a Context");

            cmdqueue = clCreateCommandQueue(context, id, 0, &errc);
            XAI_COMPUTATION_LOG(cmdqueue, "XAI::COMPUTATION: Failed to create a CommandQueue");

            program = clCreateProgramWithSource(context, 1, (const char**)&buffer, NULL, &errc);
            XAI_COMPUTATION_LOG(program, "XAI::COMPUTATION: Failed to create the Program");
            XAI_COMPUTATION_LOG(clBuildProgram(program, 0, NULL, NULL, NULL, NULL), "XAI::COMPUTATION: Failed to build the program");

            kernel = clCreateKernel(program, mainFunctionName, &errc);
            XAI_COMPUTATION_LOG(kernel, "XAI::COMPUTATION: Couldn't create the kernel");
        }
    } ComputeKernel;


}