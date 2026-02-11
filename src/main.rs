#![no_std]
#![no_main]

use core::{arch::asm, panic::PanicInfo};

use crate::graphics::fill_blue;

mod graphics;

/// Syscall 0 = Plot Pixel
pub fn syscall(syscall_num: i64, arg1: i64, arg2: i64, arg3: i64) -> u64 {
    let ret: u64;
    unsafe {
        asm!(
            "syscall",
            inlateout("rax") syscall_num => ret,
            in("rdi") arg1,
            in("rsi") arg2,
            in("rdx") arg3,
            lateout("rcx") _,
            lateout("r11") _,
            options(nostack, preserves_flags)
        );
    }
    ret
}

#[unsafe(no_mangle)]
extern "C" fn _start() -> ! {
    fill_blue();
    loop {
        
    }
}

#[cfg(not(test))]
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    let msg = "Error has occurred in the userspace";
    syscall(2, msg.as_ptr() as i64, msg.len() as i64, 0);
    loop {}
}