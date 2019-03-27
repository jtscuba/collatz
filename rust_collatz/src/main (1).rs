#![feature(i128_type)]
extern crate num_cpus;
#[macro_use]
extern crate lazy_static;
extern crate threadpool;

use std::thread;
use std::sync::mpsc;
use std::env;
use std::process;
use std::cmp;
use std::ops;
use threadpool::ThreadPool;

type NumType = u128;

const BASE_BITS: u32 = 16;
const BASE_BIT_MASK: u128 = 0xFFFF;

lazy_static! {
    static ref MULTI_STEP_TABLE: Vec<MultiStepEntry> = build_multi_step_table(BASE_BITS);
}

#[derive(Debug, Clone)]
struct MultiStepEntry {
    multiplier: u32,
    constant: u32,
}

fn collatz(i: NumType) -> NumType {
    let mut current_value: NumType = i;
    let mut num_steps : NumType = 0;

    while current_value != 1 {
        if current_value < i {
            // We've already checked this number
            // so we can just terminate early
            break;
        }

        if current_value % 2 == 0 {
            current_value = current_value / 2;
        } else {
            current_value = current_value
                .checked_mul(3).unwrap()
                .checked_add(1).unwrap()
                .checked_div(2).unwrap();
        }
        num_steps += 1;
    }

    return num_steps
}

fn collatz_range(
    start: NumType,
    end: NumType,
    collatz_impl: fn(NumType) -> NumType) -> (NumType, NumType) {
    let mut max_steps_input = 1;
    let mut max_steps = 0;
    for i in start..end {
        let steps = collatz_impl(i);

        if steps > max_steps {
            max_steps = steps;
            max_steps_input = i;
        }
    }
    (max_steps_input, max_steps)
}

fn multi_step_collatz(i: NumType) -> NumType {
    let mut current_value: NumType = i;
    let mut num_steps : NumType = 0;

    // start at five b/c of 1 -> 2 -> 4 cycle with table implementation
    if i < 5 {
        return num_steps;
    }

    while current_value >= i {
        let lower_bits = current_value & BASE_BIT_MASK;
        let upper_bits = current_value >> BASE_BITS;
        let multi_step_entry = &MULTI_STEP_TABLE[lower_bits as usize];

        let upper_res = upper_bits.checked_mul(multi_step_entry.multiplier as NumType).unwrap();
        current_value = upper_res.checked_add(multi_step_entry.constant as NumType).unwrap();

        // println!("{:?}", current_value);
        num_steps += 1;
    }

    num_steps
}

fn build_multi_step_table(base_bits: u32) -> Vec<MultiStepEntry> {
    let max_base_value = 1 << base_bits;
    let default_entry = MultiStepEntry{multiplier: 0, constant: 0};
    let mut multi_step_table = vec![default_entry; max_base_value];

    for lower_bit_value in 0..max_base_value {
        let mut multiplier = 2u32.pow(base_bits);
        let mut constant = lower_bit_value as u32;

        while multiplier % 2 == 0 {
            if constant % 2 == 0 {
                multiplier = multiplier / 2;
                constant   = constant / 2;
            }
            else {
                multiplier = multiplier.checked_mul(3).unwrap();
                constant   = constant.checked_mul(3).unwrap().checked_add(1).unwrap();
            }
        }

        multi_step_table[lower_bit_value] = MultiStepEntry{multiplier, constant};
    }

    multi_step_table
}

fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() != 2 {
        println!("usage: rust_collatz <power of 2 to check up to>");
        process::exit(0);
    }
    let power = args[1].parse::<i32>().unwrap();
    let max: NumType  = 1 << power;

    let num_worker_threads = num_cpus::get();
    let num_chunks = (num_worker_threads  s  ) as NumType;
    let (tx, rx) = mpsc::channel::<(NumType, NumType)>();

    let pool = ThreadPool::new(num_worker_threads);

    // worker threads
    for chunk in 0..num_chunks {
        let tx_clone = mpsc::Sender::clone(&tx);
        thread::spawn(move || {
            let chunk_size = max / num_chunks;
            let start = cmp::max(chunk_size * chunk, 1);
            let end = cmp::max(chunk_size * (chunk+1), max+1);

            let res = collatz_range(start, end, multi_step_collatz);

            tx_clone.send(res).unwrap();
        });
    }

    // process the workers results
    let mut max_steps_input: NumType = 0;
    let mut max_steps: NumType = 0;

    for _ in 0..num_chunks {
        let (input, steps) = rx.recv().unwrap();
        if steps > max_steps {
            max_steps = steps;
            max_steps_input = input;
        }
    }

    println!("{} took {} steps", max_steps_input, max_steps)
}
