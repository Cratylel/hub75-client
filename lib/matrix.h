#ifndef FRAME_MAPPER_H
#define FRAME_MAPPER_H

namespace pixel_mapper
{
    struct pixel
    {
        bool R;
        bool G;
        bool B;
    };

    struct frame
    {
        uint32_t width;
        uint32_t height;
        pixel *fb;
    };
}

namespace framebuffer
{
    struct buffer
    {
        uint32_t lock = 0; // optional, mostly unused after using pthread mutex
        pixel_mapper::frame frame;
    };
}
class matrix
{
public:
    framebuffer::buffer *map_buffer(char *buffer_path, uint32_t width, uint32_t height);
    void unmap_buffer(uint32_t width, uint32_t height);
    // void lock_mutex();
    // void unlock_mutex();

    framebuffer::buffer *create_local_buffer(uint32_t width, uint32_t height);

    void start_transaction();
    void end_transaction();
    void start_write();
    void end_write();

    void set_active_buffer(framebuffer::buffer *buf);

    void flush_buffer(framebuffer::buffer* buf);

    void setPixel(int x, int y, bool R, bool G, bool B);
    
    void drawRect(int x, int y, int width, int height, bool R, bool G, bool B);
    void drawFillRect(int x, int y, int width, int height, bool R, bool G, bool B);
    void fillScreen(bool R, bool G, bool B);
    void drawLine(int x1, int y1, int x2, int y2, bool R, bool G, bool B);
    void drawCircle(int xc, int yc, int radius, bool R, bool G, bool B);
    void drawFillCircle(int xc, int yc, int radius, bool R, bool G, bool B);
    


}; // class matrix

#endif // FRAME_MAPPER_H
