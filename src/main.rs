use humantime::format_duration;
use std::env;
use std::process::Command;
use std::time::{Duration, Instant};

fn run(src: &str) -> std::time::Duration {
    let start = Instant::now();

    Command::new(src).status().unwrap();

    start.elapsed()
}

fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() < 2 || args.len() > 3 {
        eprintln!("usage: {} <file1> [file2]", args[0]);
        std::process::exit(1);
    }

    let n = 20;

    // file 1
    let _ = run(&args[1]);
    let mut total1 = Duration::ZERO;
    for _ in 0..n {
        total1 += run(&args[1]);
    }
    let d1 = total1 / n as u32;

    if args.len() == 2 {
        println!("{}", format_duration(d1));
    } else {
        let _ = run(&args[2]);
        let mut total2 = Duration::ZERO;
        for _ in 0..n {
            total2 += run(&args[2]);
        }
        let d2 = total2 / n as u32;

        println!("{} | {}", format_duration(d1), format_duration(d2));
    }
}
