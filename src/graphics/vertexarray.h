#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include "graphics/opengl_utils.h"
#include "core/core.h"

#define VERTEX_ARRAY_BUFFER_CAPACITY 5

struct VertexBuffer {
    GLuint handle;
    u32 element_size;
    u32 usage;
    u32 type;
};
typedef struct VertexBuffer VertexBuffer;

struct VertexArray {
    GLuint handle;
    VertexBuffer vertex_buffers[VERTEX_ARRAY_BUFFER_CAPACITY];
    u32 vertex_buffer_count;
    u32 vertices_count;
};
typedef struct VertexArray VertexArray;

boolean vertexarray_initialise(VertexArray* vertex_array);
boolean vertexarray_addbufferf(VertexArray* vertex_array, float* values, u32 length, u32 usage, u32 element_size, u32 byte_stride);
boolean vertexarray_addbufferi(VertexArray* vertex_array, int* values, u32 length, u32 usage, u32 element_size);

boolean vertexarray_buffersubdata_f(VertexArray* vertex_array, u32 buffer_index, float* data, u32 length);
boolean vertexarray_buffersubdata_i(VertexArray* vertex_array, u32 buffer_index, int* data, u32 length);

void vertexarray_bind(VertexArray* vertex_array);
void vertexarray_destroy(VertexArray* vertex_array);


#endif