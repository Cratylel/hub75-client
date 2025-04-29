
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>
#include "../../lib/matrix.h"

#define SHM_NAME "/pixel_mapper_buffer"

matrix m;

// this example writes some shapes to a 64x32 matrix with the buffer at SHM_NAME

int main() {
    framebuffer::buffer* buffer = m.map_buffer(SHM_NAME, 64, 32);
    m.set_active_buffer(buffer);
    m.fillScreen(0, 0, 0);
    m.drawRect(0, 0, 10, 10, 1, 0, 0);
    m.drawFillRect(10, 10, 5, 5, 0, 1, 0);
    m.drawLine(0, 0, 64, 32, 0, 0, 1);
    m.drawLine(64/2, 0, 64/2, 32, 1, 1, 0);
    m.drawLine(0, 16, 64, 16, 0, 1, 1);
    m.drawCircle(32, 16, 5, 1, 0, 1);
    m.drawFillCircle(32, 16, 3, 1, 0, 1);
    m.setPixel(12, 1, 0, 1, 1);
    m.unmap_buffer(64, 32);
    return 0;
}   
