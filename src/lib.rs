#![no_std]

extern crate alloc;

use alloc::vec::Vec;
use core::cell::UnsafeCell;
use limine::request::FramebufferRequest;

unsafe extern "C" {
    static FRAMEBUFFER_REQUEST: FramebufferRequest;
}

pub struct FramebufferInfo {
    pub ptr: *mut u32,
    pub width: usize,
    pub height: usize,
    pub pitch: usize,
}

struct DisplayCell {
    inner: UnsafeCell<Option<Display>>,
}

unsafe impl Sync for DisplayCell {}

impl DisplayCell {
    const fn new() -> Self {
        Self {
            inner: UnsafeCell::new(None),
        }
    }

    fn set(&self, display: Display) {
        unsafe {
            *self.inner.get() = Some(display);
        }
    }

    fn get(&self) -> &mut Display {
        unsafe {
            (*self.inner.get())
                .as_mut()
                .expect("Display not initialized")
        }
    }
}

static DISPLAY: DisplayCell = DisplayCell::new();

pub struct Display {
    fb: FramebufferInfo,
    back_buffer: Vec<u32>,
}

impl Display {
    pub fn init() {
        let fb = FramebufferInfo::get();
        let size = fb.width * fb.height;
        let back_buffer = alloc::vec![0u32; size];
        
        DISPLAY.set(Self { fb, back_buffer });
    }

    pub fn get() -> &'static mut Self {
        DISPLAY.get()
    }

    #[inline]
    pub fn set_pixel(&mut self, x: usize, y: usize, color: u32) {
        if x < self.fb.width && y < self.fb.height {
            self.back_buffer[y * self.fb.width + x] = color;
        }
    }

    #[inline]
    pub fn get_pixel(&self, x: usize, y: usize) -> u32 {
        if x < self.fb.width && y < self.fb.height {
            self.back_buffer[y * self.fb.width + x]
        } else {
            0
        }
    }

    pub fn swap_buffers(&mut self) {
        unsafe {
            core::ptr::copy_nonoverlapping(
                self.back_buffer.as_ptr(),
                self.fb.ptr,
                self.back_buffer.len(),
            );
        }
    }

    pub fn clear(&mut self, color: u32) {
        self.back_buffer.fill(color);
    }

    pub fn width(&self) -> usize {
        self.fb.width
    }

    pub fn height(&self) -> usize {
        self.fb.height
    }
}

impl FramebufferInfo {
    pub fn get() -> Self {
        let response = unsafe { 
            FRAMEBUFFER_REQUEST.get_response().unwrap()
        };
        let framebuffer = response.framebuffers().next().unwrap();
        
        Self {
            ptr: framebuffer.addr() as *mut u32,
            width: framebuffer.width() as usize,
            height: framebuffer.height() as usize,
            pitch: framebuffer.pitch() as usize / 4,
        }
    }
    
    #[inline]
    pub fn set_pixel(&self, x: usize, y: usize, color: u32) {
        if x < self.width && y < self.height {
            unsafe {
                let pixel_ptr = self.ptr.add(y * self.pitch + x);
                *pixel_ptr = color;
            }
        }
    }
}

pub fn rgb(r: u8, g: u8, b: u8) -> u32 {
    ((r as u32) << 16) | ((g as u32) << 8) | (b as u32)
}

pub fn draw_pixel(x: usize, y: usize, color: u32) {
    Display::get().set_pixel(x, y, color);
}

pub fn clear_screen(color: u32) {
    Display::get().clear(color);
}

pub fn draw_line(x0: isize, y0: isize, x1: isize, y1: isize, color: u32) {
    let display = Display::get();
    
    let dx = (x1 - x0).abs();
    let dy = -(y1 - y0).abs();
    let sx = if x0 < x1 { 1 } else { -1 };
    let sy = if y0 < y1 { 1 } else { -1 };
    let mut err = dx + dy;
    
    let mut x = x0;
    let mut y = y0;
    
    loop {
        if x >= 0 && y >= 0 {
            display.set_pixel(x as usize, y as usize, color);
        }
        
        if x == x1 && y == y1 {
            break;
        }
        
        let e2 = 2 * err;
        if e2 >= dy {
            err += dy;
            x += sx;
        }
        if e2 <= dx {
            err += dx;
            y += sy;
        }
    }
}

pub fn draw_rect(x: usize, y: usize, width: usize, height: usize, color: u32) {
    let display = Display::get();
    
    for dy in 0..height {
        for dx in 0..width {
            display.set_pixel(x + dx, y + dy, color);
        }
    }
}

pub fn draw_rect_outline(x: usize, y: usize, width: usize, height: usize, color: u32, thickness: usize) {
    let display = Display::get();
    
    for t in 0..thickness {
        for dx in 0..width {
            display.set_pixel(x + dx, y + t, color);
            if y + height > t {
                display.set_pixel(x + dx, y + height - 1 - t, color);
            }
        }
    }
    
    for t in 0..thickness {
        for dy in 0..height {
            display.set_pixel(x + t, y + dy, color);
            if x + width > t {
                display.set_pixel(x + width - 1 - t, y + dy, color);
            }
        }
    }
}

pub fn draw_circle(cx: isize, cy: isize, radius: isize, color: u32) {
    let display = Display::get();
    
    let mut x = radius;
    let mut y = 0;
    let mut err = 0;
    
    while x >= y {
        if cx + x >= 0 && cy + y >= 0 {
            display.set_pixel((cx + x) as usize, (cy + y) as usize, color);
        }
        if cx + y >= 0 && cy + x >= 0 {
            display.set_pixel((cx + y) as usize, (cy + x) as usize, color);
        }
        if cx - y >= 0 && cy + x >= 0 {
            display.set_pixel((cx - y) as usize, (cy + x) as usize, color);
        }
        if cx - x >= 0 && cy + y >= 0 {
            display.set_pixel((cx - x) as usize, (cy + y) as usize, color);
        }
        if cx - x >= 0 && cy - y >= 0 {
            display.set_pixel((cx - x) as usize, (cy - y) as usize, color);
        }
        if cx - y >= 0 && cy - x >= 0 {
            display.set_pixel((cx - y) as usize, (cy - x) as usize, color);
        }
        if cx + y >= 0 && cy - x >= 0 {
            display.set_pixel((cx + y) as usize, (cy - x) as usize, color);
        }
        if cx + x >= 0 && cy - y >= 0 {
            display.set_pixel((cx + x) as usize, (cy - y) as usize, color);
        }
        
        y += 1;
        err += 1 + 2 * y;
        if 2 * (err - x) + 1 > 0 {
            x -= 1;
            err += 1 - 2 * x;
        }
    }
}

pub fn fill_circle(cx: isize, cy: isize, radius: isize, color: u32) {
    let display = Display::get();
    
    for y in -radius..=radius {
        for x in -radius..=radius {
            if x * x + y * y <= radius * radius {
                if cx + x >= 0 && cy + y >= 0 {
                    display.set_pixel((cx + x) as usize, (cy + y) as usize, color);
                }
            }
        }
    }
}

pub fn draw_rounded_rect(x: usize, y: usize, width: usize, height: usize, radius: usize, color: u32) {
    let display = Display::get();
    
    for dy in radius..height.saturating_sub(radius) {
        for dx in 0..width {
            display.set_pixel(x + dx, y + dy, color);
        }
    }
    
    for dy in 0..radius {
        for dx in radius..width.saturating_sub(radius) {
            display.set_pixel(x + dx, y + dy, color);
            display.set_pixel(x + dx, y + height - 1 - dy, color);
        }
    }
    
    let r = radius as isize;
    for cy in 0..=r {
        for cx in 0..=r {
            if cx * cx + cy * cy <= r * r {
                display.set_pixel(x + radius - cx as usize, y + radius - cy as usize, color);
                if width > radius {
                    display.set_pixel(x + width - radius - 1 + cx as usize, y + radius - cy as usize, color);
                }
                if height > radius {
                    display.set_pixel(x + radius - cx as usize, y + height - radius - 1 + cy as usize, color);
                }
                if width > radius && height > radius {
                    display.set_pixel(x + width - radius - 1 + cx as usize, y + height - radius - 1 + cy as usize, color);
                }
            }
        }
    }
}

pub fn draw_gradient_vertical(x: usize, y: usize, width: usize, height: usize, color_top: u32, color_bottom: u32) {
    let display = Display::get();
    
    let r1 = ((color_top >> 16) & 0xFF) as isize;
    let g1 = ((color_top >> 8) & 0xFF) as isize;
    let b1 = (color_top & 0xFF) as isize;
    
    let r2 = ((color_bottom >> 16) & 0xFF) as isize;
    let g2 = ((color_bottom >> 8) & 0xFF) as isize;
    let b2 = (color_bottom & 0xFF) as isize;
    
    for dy in 0..height {
        let ratio = dy as isize * 256 / height as isize;
        let r = ((r1 * (256 - ratio) + r2 * ratio) / 256) as u32;
        let g = ((g1 * (256 - ratio) + g2 * ratio) / 256) as u32;
        let b = ((b1 * (256 - ratio) + b2 * ratio) / 256) as u32;
        let color = (r << 16) | (g << 8) | b;
        
        for dx in 0..width {
            display.set_pixel(x + dx, y + dy, color);
        }
    }
}

pub fn draw_gradient_horizontal(x: usize, y: usize, width: usize, height: usize, color_left: u32, color_right: u32) {
    let display = Display::get();
    
    let r1 = ((color_left >> 16) & 0xFF) as isize;
    let g1 = ((color_left >> 8) & 0xFF) as isize;
    let b1 = (color_left & 0xFF) as isize;
    
    let r2 = ((color_right >> 16) & 0xFF) as isize;
    let g2 = ((color_right >> 8) & 0xFF) as isize;
    let b2 = (color_right & 0xFF) as isize;
    
    for dx in 0..width {
        let ratio = dx as isize * 256 / width as isize;
        let r = ((r1 * (256 - ratio) + r2 * ratio) / 256) as u32;
        let g = ((g1 * (256 - ratio) + g2 * ratio) / 256) as u32;
        let b = ((b1 * (256 - ratio) + b2 * ratio) / 256) as u32;
        let color = (r << 16) | (g << 8) | b;
        
        for dy in 0..height {
            display.set_pixel(x + dx, y + dy, color);
        }
    }
}

pub fn draw_triangle(x0: isize, y0: isize, x1: isize, y1: isize, x2: isize, y2: isize, color: u32) {
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

pub fn draw_bitmap(x: usize, y: usize, width: usize, height: usize, bitmap: &[u8], color: u32) {
    let display = Display::get();
    
    for dy in 0..height {
        for dx in 0..width {
            let byte_index = (dy * width + dx) / 8;
            let bit_index = 7 - ((dy * width + dx) % 8);
            
            if byte_index < bitmap.len() {
                if (bitmap[byte_index] >> bit_index) & 1 == 1 {
                    display.set_pixel(x + dx, y + dy, color);
                }
            }
        }
    }
}

pub fn blend_colors(bg: u32, fg: u32, alpha: u8) -> u32 {
    let alpha = alpha as u32;
    let inv_alpha = 255 - alpha;
    
    let bg_r = (bg >> 16) & 0xFF;
    let bg_g = (bg >> 8) & 0xFF;
    let bg_b = bg & 0xFF;
    
    let fg_r = (fg >> 16) & 0xFF;
    let fg_g = (fg >> 8) & 0xFF;
    let fg_b = fg & 0xFF;
    
    let r = (bg_r * inv_alpha + fg_r * alpha) / 255;
    let g = (bg_g * inv_alpha + fg_g * alpha) / 255;
    let b = (bg_b * inv_alpha + fg_b * alpha) / 255;
    
    (r << 16) | (g << 8) | b
}

pub fn draw_shadow(x: usize, y: usize, width: usize, height: usize, offset: usize, blur: usize) {
    let display = Display::get();
    
    for dy in 0..height + blur * 2 {
        for dx in 0..width + blur * 2 {
            let shadow_x = x + offset + dx;
            let shadow_y = y + offset + dy;
            
            let dist_x = if dx < blur { blur - dx } else if dx >= width + blur { dx - width - blur + 1 } else { 0 };
            let dist_y = if dy < blur { blur - dy } else if dy >= height + blur { dy - height - blur + 1 } else { 0 };
            let dist = dist_x.max(dist_y);
            
            if dist <= blur {
                let alpha = ((blur - dist) * 128 / blur) as u8;
                if shadow_x < display.fb.width && shadow_y < display.fb.height {
                    let bg = display.get_pixel(shadow_x, shadow_y);
                    display.set_pixel(shadow_x, shadow_y, blend_colors(bg, 0x000000, alpha));
                }
            }
        }
    }
}

pub fn swap_buffers() {
    Display::get().swap_buffers();
}

pub mod colors {
    pub const BLACK: u32 = 0x000000;
    pub const WHITE: u32 = 0xFFFFFF;
    pub const RED: u32 = 0xFF0000;
    pub const GREEN: u32 = 0x00FF00;
    pub const BLUE: u32 = 0x0000FF;
    pub const YELLOW: u32 = 0xFFFF00;
    pub const CYAN: u32 = 0x00FFFF;
    pub const MAGENTA: u32 = 0xFF00FF;
    pub const GRAY: u32 = 0x808080;
    pub const DARK_GRAY: u32 = 0x404040;
    pub const LIGHT_GRAY: u32 = 0xC0C0C0;
    pub const ORANGE: u32 = 0xFF8000;
    pub const PURPLE: u32 = 0x8000FF;
}

pub mod ui;