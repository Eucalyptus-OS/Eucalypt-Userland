#include <stdint.h>
#include <vfs.h>
#include <compositor.h>

#define PSF1_MAGIC 0x0436
#define PSF2_MAGIC 0x864AB572
#define PSF1_MODE512 0x01
#define FONT_BUF_SIZE (16 * 1024)

typedef struct {
    uint16_t magic;
    uint8_t  mode;
    uint8_t  charsize;
} psf1_header_t;

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t headersize;
    uint32_t flags;
    uint32_t numglyph;
    uint32_t bytesperglyph;
    uint32_t height;
    uint32_t width;
} psf2_header_t;

typedef enum { FONT_NONE, FONT_PSF1, FONT_PSF2 } font_kind_t;

static struct {
    font_kind_t kind;
    const uint8_t *glyphs;
    uint32_t glyph_w;
    uint32_t glyph_h;
    uint32_t bytes_per_glyph;
    uint32_t bytes_per_row;
    uint32_t num_glyphs;
} font;

static char font_data[FONT_BUF_SIZE];

int init_font_renderer() {
    int fd = open("A:F.PSF", O_RDONLY, 0);
    if (fd < 0) return -1;

    long n = read(fd, font_data, FONT_BUF_SIZE);
    close(fd);
    if (n < 4) return -2;

    psf2_header_t *h2 = (psf2_header_t *)font_data;
    if (h2->magic == PSF2_MAGIC) {
        if ((uint32_t)n < h2->headersize) return -1;  // guard against truncated read
        font.kind            = FONT_PSF2;
        font.glyph_w         = h2->width;
        font.glyph_h         = h2->height;
        font.bytes_per_glyph = h2->bytesperglyph;
        font.num_glyphs      = h2->numglyph;
        font.bytes_per_row   = (h2->width + 7) / 8;
        font.glyphs          = (const uint8_t *)font_data + h2->headersize;
        return 0;
    }

    psf1_header_t *h1 = (psf1_header_t *)font_data;
    if (h1->magic == PSF1_MAGIC) {
        if (h1->charsize == 0) return -1;
        uint32_t expected = sizeof(psf1_header_t) + 256 * h1->charsize;
        if ((uint32_t)n < expected) return -1;  // truncated
        font.kind            = FONT_PSF1;
        font.glyph_w         = 8;
        font.glyph_h         = h1->charsize;
        font.bytes_per_glyph = h1->charsize;
        font.num_glyphs      = (h1->mode & PSF1_MODE512) ? 512 : 256;
        font.bytes_per_row   = 1;
        font.glyphs          = (const uint8_t *)font_data + sizeof(psf1_header_t);
        return 0;
    }

    return -4;
}

static void draw_glyph(context_t *ctx, uint32_t cp,
                        uint32_t px, uint32_t py,
                        uint32_t fg, uint32_t bg, int transparent_bg) {
    if (font.kind == FONT_NONE) return;
    if (cp >= font.num_glyphs) cp = '?';

    const uint8_t *glyph = font.glyphs + cp * font.bytes_per_glyph;

    for (uint32_t row = 0; row < font.glyph_h; row++) {
        const uint8_t *rowbytes = glyph + row * font.bytes_per_row;
        for (uint32_t col = 0; col < font.glyph_w; col++) {
            int set = (rowbytes[col / 8] >> (7 - (col % 8))) & 1;
            if (set)
                context_put_pixel(ctx, px + col, py + row, fg);
            else if (!transparent_bg)
                context_put_pixel(ctx, px + col, py + row, bg);
        }
    }
}

void render_char(context_t *ctx, char c,
                 uint32_t x, uint32_t y,
                 uint32_t fg, uint32_t bg, int transparent_bg) {
    draw_glyph(ctx, (uint32_t)(unsigned char)c, x, y, fg, bg, transparent_bg);
}

void render_text(context_t *ctx, const char *text,
                 uint32_t x, uint32_t y,
                 uint32_t fg, uint32_t bg, int transparent_bg) {
    if (!ctx || !text || font.kind == FONT_NONE) return;

    uint32_t cx = x;
    uint32_t cy = y;

    for (; *text; text++) {
        if (*text == '\n') {
            cx  = x;
            cy += font.glyph_h;
            continue;
        }
        if (*text == '\r') {
            cx = x;
            continue;
        }
        if (cx + font.glyph_w > ctx->width) {
            cx  = x;
            cy += font.glyph_h;
        }
        if (cy + font.glyph_h > ctx->height) break;

        draw_glyph(ctx, (uint32_t)(unsigned char)*text, cx, cy, fg, bg, transparent_bg);
        cx += font.glyph_w;
    }
}

uint32_t font_glyph_width()  { return font.glyph_w; }
uint32_t font_glyph_height() { return font.glyph_h; }