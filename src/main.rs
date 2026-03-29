use humantime::format_duration;
use std::env;
use std::process::Command;
use std::time::Instant;

fn run(src: &str) -> std::time::Duration {
    let start = Instant::now();

    Command::new(src).status().unwrap();

    start.elapsed()
}

fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() < 2 || args.len() > 3 {
        eprintln!("usage: {} <file1.c> [file2.c]", args[0]);
        std::process::exit(1);
    }

    let d1 = run(&args[1]);

    if args.len() == 2 {
        println!("{}", format_duration(d1));
    } else {
        let d2 = run(&args[2]);
        println!("{} | {}", format_duration(d1), format_duration(d2));
    }
}
