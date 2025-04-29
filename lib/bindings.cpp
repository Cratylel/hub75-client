#include <pybind11/pybind11.h>
#include "matrix.h"

namespace py = pybind11;
matrix mx;

void flush_buffer(py::buffer input) {
    py::buffer_info info = input.request();

    if (info.ndim != 3) {
        throw std::runtime_error("Expected 3D input (height, width, channels)");
    }
    if (info.shape[2] < 3) {
        throw std::runtime_error("Expected at least 3 channels (RGB)");
    }

    uint32_t width = static_cast<uint32_t>(info.shape[1]);
    uint32_t height = static_cast<uint32_t>(info.shape[0]);
    
    auto* temp_buffer = new framebuffer::buffer();
    temp_buffer->frame.width = width;
    temp_buffer->frame.height = height;
    temp_buffer->frame.fb = new pixel_mapper::pixel[width * height];

    uint8_t* data = static_cast<uint8_t*>(info.ptr);

    // Loop over the image and copy with horizontal flip
    for (uint32_t y = 0; y < height; ++y) {
        for (uint32_t x = 0; x < width; ++x) {
            uint32_t src_x = x;
            uint32_t src_y = y;
            uint32_t dst_x = width - 1 - x; // flip horizontally
            uint32_t dst_y = y;

            uint32_t src_index = (src_y * width + src_x) * 3;
            uint32_t dst_index = dst_y * width + dst_x;

            uint8_t R = *(data + src_y * info.strides[0] + src_x * info.strides[1] + 0 * info.strides[2]);
            uint8_t G = *(data + src_y * info.strides[0] + src_x * info.strides[1] + 1 * info.strides[2]);
            uint8_t B = *(data + src_y * info.strides[0] + src_x * info.strides[1] + 2 * info.strides[2]);

            temp_buffer->frame.fb[dst_index].R = R > 127;
            temp_buffer->frame.fb[dst_index].G = G > 127;
            temp_buffer->frame.fb[dst_index].B = B > 127;
        }
    }

    // Call your real function
    mx.flush_buffer(temp_buffer);

    // Cleanup
    delete[] temp_buffer->frame.fb;
    delete temp_buffer;
}

void map_buffer(char* path, uint32_t width, uint32_t height) {
    mx.set_active_buffer(mx.map_buffer(path, width, height));
}

PYBIND11_MODULE(matrix, m) {
    m.def("flush_buffer", &flush_buffer, "Flush buffer");
    m.def("map_buffer", &map_buffer, "Map shared memory buffer");
}