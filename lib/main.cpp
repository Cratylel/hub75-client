
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>
#include "matrix.h"
#define SHM_NAME "/pixel_mapper_buffer"

matrix m;

int main() {

    //frame_mapper::buffer *buffer = frame_mapper::map_buffer("/pixel_mapper_buffer", 64, 32);
    //frame_mapper::map_buffer("/pixel_mapper_buffer", 64, 32);
    // pthread_mutex_lock(frame_mapper::get_mutex());
    // pthread_mutex_unlock(frame_mapper::get_mutex());
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
    m.start_write();
    buffer->frame.fb[0].R = 1;
    buffer->frame.fb[2047].B = 1;
    m.end_write();

    framebuffer::buffer* buf2 = m.create_local_buffer(64, 32);
    buf2->frame.fb[0].B = 1;
    buf2->frame.fb[2047].G = 1;
    m.flush_buffer(buf2);
    buf2->frame.fb[1].R = 1;
    //frame_mapper::lock_mutex();
    // for (int i = 1; i < 2048; i++) {
    //     m.start_write();
        
    //     buffer->frame.fb[i-1].R = 0;
    //     buffer->frame.fb[i-1].G = 0;
    //     buffer->frame.fb[i-1].B = 0;
    //     buffer->frame.fb[i].R = 1;
    //     buffer->frame.fb[i].G = 1;
    //     buffer->frame.fb[i].B = 1;
    //     m.end_write();
    //     usleep(10000);
    //     // buffer->frame.fb[i].R = rand() % 2;
    //     // buffer->frame.fb[i].G = rand() % 2;
    //     // buffer->frame.fb[i].B = rand() % 2;
        
    // }
   // buffer->frame.fb[1023].B = 1;
    //buffer->frame.fb[0].G = 0;
    //frame_mapper::unlock_mutex();
    m.unmap_buffer(64, 32);
    return 0;
}   
