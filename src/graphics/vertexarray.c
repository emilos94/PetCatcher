#include "graphics/vertexarray.h"

boolean vertexarray_initialise(VertexArray* vertex_array) {
    GL_CALL(glGenVertexArrays(1, &vertex_array->handle));
    GL_CALL(glBindVertexArray(vertex_array->handle));
    return true;
}

boolean vertexarray_addbufferf(VertexArray* vertex_array, float* values, u32 length, u32 usage, u32 element_size) {
    if (!vertex_array || vertex_array->vertex_buffer_count >= VERTEX_ARRAY_BUFFER_CAPACITY) {
        return false;
    }

    GL_CALL(glBindVertexArray(vertex_array->handle));

    VertexBuffer* vertex_buffer = vertex_array->vertex_buffers + vertex_array->vertex_buffer_count;
    vertex_buffer->element_size = element_size;
    vertex_buffer->usage = usage;

    GL_CALL(glGenBuffers(1, &vertex_buffer->handle));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->handle));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * length, values, usage));

    GL_CALL(glVertexAttribPointer(vertex_array->vertex_buffer_count, element_size, GL_FLOAT, GL_FALSE, element_size * sizeof(float), (void*)0));
    GL_CALL(glEnableVertexAttribArray(vertex_array->vertex_buffer_count));

    vertex_array->vertex_buffer_count++;

    return true;
}

boolean vertexarray_addbufferi(VertexArray* vertex_array, int* values, u32 length, u32 usage, u32 element_size);


void vertexarray_bind(VertexArray* vertex_array) {
    GL_CALL(glBindVertexArray(vertex_array->handle));
}

void vertexarray_destroy(VertexArray* vertex_array) {
    for(u32 i = 0; i < vertex_array->vertex_buffer_count; i++) {
        VertexBuffer* buffer = vertex_array->vertex_buffers + i;
        GL_CALL(glDeleteBuffers(1, &buffer->handle));
    }

    GL_CALL(glDeleteVertexArrays(1, &vertex_array->handle));
}
