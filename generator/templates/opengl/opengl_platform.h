//* Copyright 2019 The Dawn Authors
//*
//* Licensed under the Apache License, Version 2.0 (the "License");
//* you may not use this file except in compliance with the License.
//* You may obtain a copy of the License at
//*
//*     http://www.apache.org/licenses/LICENSE-2.0
//*
//* Unless required by applicable law or agreed to in writing, software
//* distributed under the License is distributed on an "AS IS" BASIS,
//* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//* See the License for the specific language governing permissions and
//* limitations under the License.

#include <KHR/khrplatform.h>

using GLvoid = void;
using GLchar = char;
using GLenum = unsigned int;
using GLboolean = unsigned char;
using GLbitfield = unsigned int;
using GLbyte = khronos_int8_t;
using GLshort = short;
using GLint = int;
using GLsizei = int;
using GLubyte = khronos_uint8_t;
using GLushort = unsigned short;
using GLuint = unsigned int;
using GLfloat = khronos_float_t;
using GLclampf = khronos_float_t;
using GLdouble = double;
using GLclampd = double;
using GLfixed = khronos_int32_t;
using GLintptr = khronos_intptr_t;
using GLsizeiptr = khronos_ssize_t;
using GLhalf = unsigned short;
using GLint64 = khronos_int64_t;
using GLuint64 = khronos_uint64_t;
using GLsync = struct __GLsync*;
using GLeglImageOES = void*;
using GLDEBUGPROC = void(KHRONOS_APIENTRY*)(GLenum source,
                                            GLenum type,
                                            GLuint id,
                                            GLenum severity,
                                            GLsizei length,
                                            const GLchar* message,
                                            const void* userParam);
using GLDEBUGPROCARB = GLDEBUGPROC;
using GLDEBUGPROCKHR = GLDEBUGPROC;
using GLDEBUGPROCAMD = void(KHRONOS_APIENTRY*)(GLuint id,
                                               GLenum category,
                                               GLenum severity,
                                               GLsizei length,
                                               const GLchar* message,
                                               void* userParam);

{% for block in header_blocks %}
    // {{block.description}}
    {% for enum in block.enums %}
        #define {{enum.name}} {{enum.value}}
    {% endfor %}

    {% for proc in block.procs %}
        using {{proc.PFNGLPROCNAME()}} = {{proc.return_type}}(KHRONOS_APIENTRY *)(
            {%- for param in proc.params -%}
                {%- if not loop.first %}, {% endif -%}
                {{param.type}} {{param.name}}
            {%- endfor -%}
        );
    {% endfor %}

{% endfor%}

// GL_ANGLE_base_vertex_base_instance
// See crbug.com/dawn/1715 for why this is embedded
using PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEANGLEPROC = void(KHRONOS_APIENTRY *)(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount, GLuint baseInstance);
using PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEANGLEPROC = void(KHRONOS_APIENTRY *)(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei instanceCount, GLint baseVertex, GLuint baseInstance);
using PFNGLMULTIDRAWARRAYSINSTANCEDBASEINSTANCEANGLEPROC = void(KHRONOS_APIENTRY *)(GLenum mode, const GLint * firsts, const GLsizei * counts, const GLsizei * instanceCounts, const GLuint * baseInstances, GLsizei drawcount);
using PFNGLMULTIDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEANGLEPROC = void(KHRONOS_APIENTRY *)(GLenum mode, const GLsizei * counts, GLenum type, const GLvoid *const* indices, const GLsizei * instanceCounts, const GLint * baseVertices, const GLuint * baseInstances, GLsizei drawcount);

#undef DAWN_GL_APIENTRY
