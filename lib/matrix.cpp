#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "log.h"
#include "matrix.h"

#define SHM_NAME "/pixel_mapper_buffer"

uint32_t write_count = 0;
void *shm_ptr;
pthread_mutex_t *mutex;
framebuffer::buffer *act_buf;

static inline void lock_mutex()
{
    pthread_mutex_lock(mutex);
}

static inline void unlock_mutex()
{
    pthread_mutex_unlock(mutex);
}

// Returns a new allocated buffer
framebuffer::buffer *matrix::create_local_buffer(uint32_t width, uint32_t height)
{
    framebuffer::buffer *new_buffer = (framebuffer::buffer *)malloc(sizeof(framebuffer::buffer));
    if (new_buffer == NULL)
    {
        return NULL;
    }
    new_buffer->frame.width = width;
    new_buffer->frame.height = height;

    // Calculate the number of pixels and allocate memory for fb
    size_t num_pixels = width * height;
    new_buffer->frame.fb = (pixel_mapper::pixel *)malloc(sizeof(pixel_mapper::pixel) * num_pixels);
    if (new_buffer->frame.fb == NULL)
    {
        free(new_buffer);
        return NULL;
    }
    return new_buffer;
}

// Set the buffer that all operations will be performed on
void matrix::set_active_buffer(framebuffer::buffer *buf)
{
    start_write();
    act_buf = buf;
    end_write();
}

// Lock mutex no matter what write count is
void matrix::start_transaction()
{
    lock_mutex();
}

// Unlock mutex no matter what write count is
void matrix::end_transaction()
{
    unlock_mutex();
    write_count = 0;
}

// Locks mutex, calling this multiple times increases the write_count
void matrix::start_write()
{
    if (write_count == 0)
    {
        start_transaction();
    }
    write_count++;
}

// Unlocks mutex if write_count = 1, if write_count > 1 it decreases the count
void matrix::end_write()
{
    if (write_count == 1)
    {
        end_transaction();
    }
    write_count--;
}

// Map a shared memory buffer
framebuffer::buffer *matrix::map_buffer(char *buffer_path, uint32_t width, uint32_t height)
{
    int shm_fd = shm_open(buffer_path, O_RDWR, 0666);
    if (shm_fd == -1)
    {
        LOG_ERROR("Failed to open shared buffer");
        return nullptr;
    }

    size_t framebuffer_size = width * height * sizeof(pixel_mapper::pixel);
    size_t total_size = sizeof(pthread_mutex_t) + sizeof(framebuffer::buffer) + framebuffer_size;

    shm_ptr = mmap(nullptr, total_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED)
    {
        LOG_ERROR("Failed to map shared buffer");
        return nullptr;
    }
    mutex = (pthread_mutex_t *)shm_ptr;
    framebuffer::buffer *shared_buffer = (framebuffer::buffer *)((char *)shm_ptr + sizeof(pthread_mutex_t));
    pixel_mapper::pixel *fb = (pixel_mapper::pixel *)((char *)shm_ptr + sizeof(pthread_mutex_t) + sizeof(framebuffer::buffer));

    shared_buffer->frame.fb = fb;

    return shared_buffer;
}

// Unmap the shared buffer from memory. This function should be called when the buffer is no longer needed to free up resources.
void matrix::unmap_buffer(uint32_t width, uint32_t height)
{
    size_t framebuffer_size = width * height * sizeof(pixel_mapper::pixel);
    size_t total_size = sizeof(pthread_mutex_t) + sizeof(framebuffer::buffer) + framebuffer_size;
    munmap(shm_ptr, total_size);
}

// Take in a whole buffer and flush it
void matrix::flush_buffer(framebuffer::buffer* buf) {
    if (buf->frame.width != act_buf->frame.width || buf->frame.height != act_buf->frame.height) {
        throw std::runtime_error("Buffer dimensions do not match active buffer dimensions");
    }

    uint32_t total_pixels = buf->frame.width * buf->frame.height;
    start_write();
    for (uint32_t i = 0; i < total_pixels; ++i){
        act_buf->frame.fb[i] = buf->frame.fb[i];
        //printf("meow %d", buf->frame.fb[i].G);
    }
    end_write();
}

// Draw a filled rectangle on the screen at position (x, y) with the specified width and height. The color of the rectangle is determined by the RGB values provided.
void matrix::drawFillRect(int x, int y, int width, int height, bool R, bool G, bool B)
{
    start_write();
    int display_width = act_buf->frame.width;
    int display_height = act_buf->frame.height;

    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            int x_coord = x + w;
            int y_coord = y + h;
            if (x_coord >= 0 && x_coord < display_width && y_coord >= 0 && y_coord < display_height)
            {
                int pix_num = y_coord * display_width + (display_width - 1 - x_coord); // Final corrected assignment

                // ... rest of code here ...
                act_buf->frame.fb[pix_num].R = R;
                act_buf->frame.fb[pix_num].G = G;
                act_buf->frame.fb[pix_num].B = B;
            }
        }
    }
    end_write();
}

// Fill the entire screen with the specified color. The color is determined by the RGB values provided.
void matrix::fillScreen(bool R, bool G, bool B)
{
    start_write();
    for (int i = 0; i < 2048; i++)
    {
        act_buf->frame.fb[i].R = R;
        act_buf->frame.fb[i].G = G;
        act_buf->frame.fb[i].B = B;
    }
    end_write();
}

// Draw a line between two points on the screen (including diagonals). The color is determined by the RGB values provided.
void matrix::drawLine(int x1, int y1, int x2, int y2, bool R, bool G, bool B)
{
    start_write();
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (true)
    {
        // Calculate the pixel number and check bounds
        int x_coord = x1;
        int y_coord = y1;
        if (x_coord >= 0 && x_coord < act_buf->frame.width && y_coord >= 0 && y_coord < act_buf->frame.height)
        {
            int pix_num = y_coord * act_buf->frame.width + (act_buf->frame.width - 1 - x_coord);
            act_buf->frame.fb[pix_num].R = R;
            act_buf->frame.fb[pix_num].G = G;
            act_buf->frame.fb[pix_num].B = B;
        }

        if (x1 == x2 && y1 == y2)
            break;
        int e2 = 2 * err;

        if (e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
    end_write();
}

// Draw a rectangle with top left corner at (x, y) and dimensions width and height
void matrix::drawRect(int x, int y, int width, int height, bool R, bool G, bool B)
{
    start_write();
    int x2 = x + width;
    int y2 = y + height;
    if (x2 > act_buf->frame.width) {
        x2 = act_buf->frame.width;
    }
    if (y2 > act_buf->frame.height) {
        y2 = act_buf->frame.height;
    }
    drawLine(x, y, x2, y, R, G, B);
    drawLine(x2, y, x2, y2, R, G, B);
    drawLine(x2, y2, x, y2, R, G, B);
    drawLine(x, y2, x, y, R, G, B);
    end_write();
}

// Draw a circle with center at (xc, yc) and radius
void matrix::drawCircle(int xc, int yc, int radius, bool R, bool G, bool B)
{
    start_write();
    int x = radius;
    int y = 0;
    int p = 1 - radius;

    auto plotCirclePoints = [&](int x_offset, int y_offset)
    {
        std::vector<std::pair<int, int>> points = {
            {x_offset + xc, y_offset + yc},
            {y_offset + xc, x_offset + yc},
            {-x_offset + xc, y_offset + yc},
            {-y_offset + xc, x_offset + yc},
            {-x_offset + xc, -y_offset + yc},
            {-y_offset + xc, -x_offset + yc},
            {x_offset + xc, -y_offset + yc},
            {y_offset + xc, -x_offset + yc}};
        for (auto [x_coord, y_coord] : points)
        {
            if (x_coord >= 0 && x_coord < act_buf->frame.width && y_coord >= 0 && y_coord < act_buf->frame.height)
            {
                int pix_num = y_coord * act_buf->frame.width + (act_buf->frame.width - 1 - x_coord);
                act_buf->frame.fb[pix_num].R = R;
                act_buf->frame.fb[pix_num].G = G;
                act_buf->frame.fb[pix_num].B = B;
            }
        }
    };

    while (x >= y)
    {
        plotCirclePoints(x, y);
        y++;
        if (p <= 0)
        {
            p = p + 2 * y + 1;
        }
        else
        {
            x--;
            p = p + 2 * y - 2 * x + 1;
        }
    }
    end_write();
}

// Fill a circle with center at (xc, yc) and radius
void matrix::drawFillCircle(int xc, int yc, int radius, bool R, bool G, bool B)
{
    start_write();
    for (int r = radius; r >= 0; r--)
    {
        drawCircle(xc, yc, r, R, G, B);
    }
    end_write();
}

// Sets a pixel at (x, y) with color (R, G, B)
void matrix::setPixel(int x, int y, bool R, bool G, bool B)
{
    start_write();
    if (x >= 0 && x < act_buf->frame.width && y >= 0 && y < act_buf->frame.height) {
        int pix_num = y * act_buf->frame.width + (act_buf->frame.width - 1 - x);
        act_buf->frame.fb[pix_num].R = R;
        act_buf->frame.fb[pix_num].G = G;
        act_buf->frame.fb[pix_num].B = B;
    }
    end_write();
}