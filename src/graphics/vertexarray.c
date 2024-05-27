#include "graphics/vertexarray.h"

boolean vertexarray_initialise(VertexArray* vertex_array) {
    GL_CALL(glGenVertexArrays(1, &vertex_array->handle));
    GL_CALL(glBindVertexArray(vertex_array->handle));
    vertex_array->vertex_buffer_count = 0;
    vertex_array->ebo_length = 0;
    return true;
}

boolean vertexarray_addbufferf(VertexArray* vertex_array, float* values, u32 length, u32 usage, u32 element_size, u32 byte_stride) {
    if (!vertex_array || vertex_array->vertex_buffer_count >= VERTEX_ARRAY_BUFFER_CAPACITY) {
        return false;
    }

    GL_CALL(glBindVertexArray(vertex_array->handle));

    VertexBuffer* vertex_buffer = vertex_array->vertex_buffers + vertex_array->vertex_buffer_count;
    vertex_buffer->element_size = element_size;
    vertex_buffer->usage = usage;
    vertex_buffer->type = GL_FLOAT;

    GL_CALL(glGenBuffers(1, &vertex_buffer->handle));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->handle));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * length, values, usage));

    GL_CALL(glVertexAttribPointer(vertex_array->vertex_buffer_count, element_size, GL_FLOAT, GL_FALSE, byte_stride, (void*)0));
    GL_CALL(glEnableVertexAttribArray(vertex_array->vertex_buffer_count));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

    vertex_array->vertex_buffer_count++;

    return true;
}

boolean vertexarray_addbufferi(VertexArray* vertex_array, int* values, u32 length, u32 usage, u32 element_size) {
    if (!vertex_array || vertex_array->vertex_buffer_count >= VERTEX_ARRAY_BUFFER_CAPACITY) {
        return false;
    }

    GL_CALL(glBindVertexArray(vertex_array->handle));

    VertexBuffer* vertex_buffer = vertex_array->vertex_buffers + vertex_array->vertex_buffer_count;
    vertex_buffer->element_size = element_size;
    vertex_buffer->usage = usage;
    vertex_buffer->type = GL_INT;

    GL_CALL(glGenBuffers(1, &vertex_buffer->handle));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->handle));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * length, values, usage));

    GL_CALL(glVertexAttribIPointer(vertex_array->vertex_buffer_count, element_size, GL_INT, 0, (void*)0));
    GL_CALL(glEnableVertexAttribArray(vertex_array->vertex_buffer_count));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

    vertex_array->vertex_buffer_count++;

    return true;
}


boolean vertexarray_buffersubdata_f(VertexArray* vertex_array, u32 buffer_index, float* data, u32 length) {
    if (buffer_index >= VERTEX_ARRAY_BUFFER_CAPACITY) {
        return false;
    }

    VertexBuffer* vbo = vertex_array->vertex_buffers + buffer_index;
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo->handle));
    GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(f32) * length, data));

    return true;
}

boolean vertexarray_buffersubdata_i(VertexArray* vertex_array, u32 buffer_index, int* data, u32 length) {
    if (buffer_index >= VERTEX_ARRAY_BUFFER_CAPACITY) {
        return false;
    }

    VertexBuffer* vbo = vertex_array->vertex_buffers + buffer_index;
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo->handle));
    GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(int) * length, data));

    return true;
}


boolean vertexarray_add_element_indices(VertexArray* vertex_array, int* indices, u32 length, u32 usage) {
    GL_CALL(glBindVertexArray(vertex_array->handle));

    GL_CALL(glGenBuffers(1, &vertex_array->ebo_handle));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_array->ebo_handle));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * length, indices, usage));
    vertex_array->ebo_length = length;
    vertex_array->vertices_count = length / 3;
    
    GL_CALL(glBindVertexArray(0));
}


void vertexarray_bind(VertexArray* vertex_array) {
    GL_CALL(glBindVertexArray(vertex_array->handle));
}


void vertexarray_unbind(void) {
    GL_CALL(glBindVertexArray(0));
}

void vertexarray_destroy(VertexArray* vertex_array) {
    for(u32 i = 0; i < vertex_array->vertex_buffer_count; i++) {
        VertexBuffer* buffer = vertex_array->vertex_buffers + i;
        GL_CALL(glDeleteBuffers(1, &buffer->handle));
    }

    if (vertex_array->ebo_length > 0) {
        GL_CALL(glDeleteBuffers(1, &vertex_array->ebo_handle));
    }

    GL_CALL(glDeleteVertexArrays(1, &vertex_array->handle));
}
