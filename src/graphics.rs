use crate::syscall;
use core::sync::atomic::{AtomicU64, Ordering};

static FRAMEBUFFER_WIDTH: AtomicU64 = AtomicU64::new(0);
static FRAMEBUFFER_HEIGHT: AtomicU64 = AtomicU64::new(0);

fn get_framebuffer_width() -> u64 {
    let width = FRAMEBUFFER_WIDTH.load(Ordering::Relaxed);
    if width == 0 {
        let actual_width = syscall(1, 0, 0, 0);
        FRAMEBUFFER_WIDTH.store(actual_width, Ordering::Relaxed);
        actual_width
    } else {
        width
    }
}

fn get_framebuffer_height() -> u64 {
    let height = FRAMEBUFFER_HEIGHT.load(Ordering::Relaxed);
    if height == 0 {
        let actual_height = syscall(1, 1, 0, 0);
        FRAMEBUFFER_HEIGHT.store(actual_height, Ordering::Relaxed);
        actual_height
    } else {
        height
    }
}

pub fn fill_screen(color: u32) {
    let width = get_framebuffer_width();
    let height = get_framebuffer_height();

    for y in 0..height {
        for x in 0..width {
            syscall(0, x as i64, y as i64, color as i64);
        }
    }
}

pub fn fill_blue() {
    fill_screen(0x0000FF);
}