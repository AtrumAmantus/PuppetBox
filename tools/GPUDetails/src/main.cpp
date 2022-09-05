#include <iostream>
#include <string>

#include <sdl2/SDL.h>
#include <glad/glad.h>

/**
* \brief Callback function for the OpenGL debug events.
*/
void
debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message,
               void const* userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
            std::cout << "Source: API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            std::cout << "Source: Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            std::cout << "Source: Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            std::cout << "Source: Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            std::cout << "Source: Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            std::cout << "Source: Other";
            break;
    }
    std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            std::cout << "Type: Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            std::cout << "Type: Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            std::cout << "Type: Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            std::cout << "Type: Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            std::cout << "Type: Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            std::cout << "Type: Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            std::cout << "Type: Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            std::cout << "Type: Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            std::cout << "Type: Other";
            break;
    }
    std::cout << std::endl;

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            std::cout << "Severity: high";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            std::cout << "Severity: medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            std::cout << "Severity: low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            std::cout << "Severity: notification";
            break;
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

bool initGfxDebug()
{
    bool success = false;

    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#ifdef _WIN64
        glDebugMessageCallback(debug_callback, nullptr);
#endif
        //                    v-- source    v-- error type v-- severity
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

        success = true;
    }

    return success;
}

bool initOpenGL()
{
    bool success = true;

    if (gladLoadGLLoader(SDL_GL_GetProcAddress))
    {
        std::cout << "OpenGL Version: " << GLVersion.major << "." << GLVersion.minor << std::endl;
        std::cout << "OpenGL Shading Language Version: " << (char*) glGetString(GL_SHADING_LANGUAGE_VERSION)
                  << std::endl;
        std::cout << "OpenGL Vendor: " << (char*) glGetString(GL_VENDOR) << std::endl;
        std::cout << "OpenGL Renderer: " << (char*) glGetString(GL_RENDERER) << std::endl;

        std::cout << "Hardware Specs:" << std::endl;

        std::int32_t intValue = 0;
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &intValue);

        std::cout << "\t" << "Max Uniform Vectors: " << intValue << std::endl;

        // Store the card's minimum UBO offset value for later.
        std::int32_t stringValue;
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &stringValue);
        std::cout << "\t" << "Minimum UBO Offset: " << static_cast<std::uint32_t>(stringValue) << std::endl;
    }
    else
    {
        success = false;
        std::cout << "Failed to initialize OpenGL api pointers" << std::endl;
    }

    return success;
}

// Required signature, Entry point is in SDL2
int main(int argc, char** argv)
{
    const std::string WINDOW_TITLE = "GPU Details";
    const std::uint32_t WINDOW_WIDTH = 800;
    const std::uint32_t WINDOW_HEIGHT = 600;

    bool error = false;

    if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
    {
        int context_flags = 0;

#ifdef _DEBUG
        context_flags |= SDL_GL_CONTEXT_DEBUG_FLAG;
#endif

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, context_flags);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        SDL_Window* window_ = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                               WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        if (window_ != nullptr)
        {
            SDL_GLContext glContext = SDL_GL_CreateContext(window_);
            if (glContext != nullptr)
            {
                SDL_GL_MakeCurrent(window_, glContext);
                SDL_GL_SetSwapInterval(0);
            }
            else
            {
                error = true;
                std::cout << "Failed to create context" << std::endl;
            }
        }
        else
        {
            error = true;
            std::cout << "Failed to create window" << std::endl;
        }
    }
    else
    {
        error = true;
        std::cout << "Failed to initialize SDL" << std::endl;
    }

    if (!error)
    {
        if (initOpenGL())
        {
#ifdef _DEBUG
            if (initGfxDebug())
            {
                std::cout << "GFX API Debugger Loaded." << std::endl;
            }
            else
            {
                std::cout << "Failed to initialize GFX debugger" << std::endl;
            }
#endif

            std::cout << "GFX API Loaded." << std::endl;
        }
        else
        {
            std::cout << "Failed to initialize GFX API" << std::endl;
        }
    }

    return 0;
}
