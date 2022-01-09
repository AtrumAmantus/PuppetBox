#pragma once

#include <functional>

namespace PB
{
    /**
    * \brief Type definition for the ProcAddress function pointer returned by the hardware library to
    * initialize the GFX API function pointers.
    */
    typedef void* (* ProcAddress)(const char* name);
}