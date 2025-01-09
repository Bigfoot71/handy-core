# Handy Core

**Handy Core** is a lightweight and straightforward collection of **C99** headers designed to gather utilities I’ve developed over the course of my personal projects. While it’s not an ambitious project, these tools might prove useful to other developers.

### Key Features
- **Full independence**: Each header is self-contained and does not rely on any other part of the collection.  
- **Simplicity**: No external dependencies, just standard C.  
- **Utility**: Lightweight solutions for common needs in C programming.  

### Contents
Here are the various modules included in Handy Core:

- **`hc_array.h`**  
  A basic dynamic array, inspired by `std::vector` in C++.

- **`hc_ease.h`**  
  A collection of easing functions for smooth animations and transitions.

- **`hc_fixed.h`**  
  Tools to manipulate fixed-point numbers in 16-bit and 32-bit formats, along with a 16-bit fractional integer type.

- **`hc_half.h`**  
  Functions to convert 32-bit floating-point numbers to 16-bit floating-point numbers (and vice versa) following the **IEEE 754** standard.

- **`hc_math.h`**  
  A library for linear math: vectors, matrices, and various useful mathematical functions.

- **`hc_string.h`**  
  A lightweight implementation of dynamic strings, similar to `std::string` in C++.

### Installation
No specific installation is required. Simply copy the necessary header files into your project, then include them in your code using `#include`.

Some headers with more complex functions, such as `hc_string.h` or `hc_array.h`, may require defining `HC_STRING_IMPL` or `HC_ARRAY_IMPL`.

### License
Handy Core is distributed under the **MIT License**. You are free to use, modify, and distribute it, as long as the license notice is retained.
