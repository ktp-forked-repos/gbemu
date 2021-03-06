#pragma once

#include "framebuffer.h"
#include "tile.h"

#include "../mmu.h"
#include "../register.h"
#include "../definitions.h"

#include <vector>
#include <memory>
#include <functional>

typedef std::function<void(const FrameBuffer&)> vblank_callback_t;

enum class VideoMode {
    ACCESS_OAM,
    ACCESS_VRAM,
    HBLANK,
    VBLANK,
};

struct TileInfo {
    u8 line;
    std::vector<u8> pixels;
};

class Video {
public:
    Video(CPU& inCPU, MMU& inMMU);

    void tick(Cycles cycles);
    void register_vblank_callback(const vblank_callback_t& _vblank_callback);

    u8 control_byte;

    ByteRegister lcd_control;
    ByteRegister lcd_status;

    ByteRegister scroll_y;
    ByteRegister scroll_x;

    /* LCDC Y-coordinate */
    ByteRegister line; /* Line y-position: register LY */
    ByteRegister ly_compare;

    ByteRegister window_y;
    ByteRegister window_x; /* Note: x - 7 */

    ByteRegister bg_palette;
    ByteRegister sprite_palette_0; /* OBP0 */
    ByteRegister sprite_palette_1; /* OBP1 */

    /* TODO: LCD Color Palettes (CGB) */
    /* TODO: LCD VRAM Bank (CGB) */

    ByteRegister dma_transfer; /* DMA */

    bool debug_disable_background = false;
    bool debug_disable_sprites = false;
    bool debug_disable_window = false;

private:
    void write_scanline(u8 current_line);
    void write_sprites();
    void draw();
    void draw_bg_line(uint current_line);
    void draw_window_line(uint current_line);
    void draw_sprite(uint sprite_n);
    u8 get_pixel_from_line(u8 byte1, u8 byte2, u8 pixel_index) const;

    bool display_enabled() const;
    bool window_tile_map() const;
    bool window_enabled() const;
    bool bg_window_tile_data() const;
    bool bg_tile_map_display() const;
    bool sprite_size() const;
    bool sprites_enabled() const;
    bool bg_enabled() const;

    TileInfo get_tile_info(Address tile_set_location, u8 tile_id, u8 tile_line) const;

    Color get_real_color(u8 pixel_value) const;
    Palette load_palette(ByteRegister& palette_register) const;
    Color get_color_from_palette(GBColor color, const Palette& palette);

    CPU& cpu;
    MMU& mmu;
    FrameBuffer buffer;
    FrameBuffer background_map;

    VideoMode current_mode = VideoMode::ACCESS_OAM;
    uint cycle_counter = 0;

    vblank_callback_t vblank_callback;
};

const uint CLOCKS_PER_HBLANK = 204; /* Mode 0 */
const uint CLOCKS_PER_SCANLINE_OAM = 80; /* Mode 2 */
const uint CLOCKS_PER_SCANLINE_VRAM = 172; /* Mode 3 */
const uint CLOCKS_PER_SCANLINE =
    (CLOCKS_PER_SCANLINE_OAM + CLOCKS_PER_SCANLINE_VRAM + CLOCKS_PER_HBLANK);

const uint CLOCKS_PER_VBLANK = 4560; /* Mode 1 */
const uint SCANLINES_PER_FRAME = 144;
const uint CLOCKS_PER_FRAME = (CLOCKS_PER_SCANLINE * SCANLINES_PER_FRAME) + CLOCKS_PER_VBLANK;
