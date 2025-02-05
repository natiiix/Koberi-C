# Kobeři-C

![Kobeřice](http://www.koberice.cz/admin/file/image/Ikony/thumbs/1.jpg)

## What is Kobeři-C
Kobeři-C is a language with Lisp-like syntax, which compiles to pure C. 

## Why Kobeři-C
Kobeři-C was built with modern features in mind. It's goal is to introduce useful modern features to the world of C, while keeping the performance on par with pure C. Kobeři-C achieves this by compiling straight to pure C.

## Features
### Simplified syntax
The syntax of Kobeři-C is simplified. Lisp-like syntax may be confusing for people coming from other languages, but in reality it's really simple. Class definition and function definition syntax doesn't differ in any way. This makes learning the syntax a piece of cake, as it follows the same guidelines for every possible statement.
### Function overloading
Kobeři-C finally introduces this well-known feature from languages like C++. While function overloading isn't such an important feature, it certainly allows the programmer to use more readable names for their functions. No more abs, fabs, fabsf.
### Automatic function declarations
Kobeři-C functions are automatically forward declared. This means you don't have to declare functions before using them. Just write the function wherever you want and call it from anywhere you want. 
### Classes with methods and inheritance
Yet another feature introduced in Kobeři-C is also well known from other languages. Classes. With methods, which accept a reference to an object. And although polymorphism has yet to be implemented, Kobeři-C does support inheritance. Create a new class and inherit all attributes and methods from an already existing class. V-tables are planned for next iterations of the Kobeři-C compiler.
### Type deduction
A small, but helpful feature, type deduction can make a programmer's life easier. Use the var keyword to define a variable and the compiler will automatically deduce the type from the value assigned to the variable. 
### Syntactic sugar
#### Print
Printing to stdout has gotten much easier and readable. No more %d's and %f's. No more puts() vs. printf(). Just call (print) and pass in everything you want to write to stdout as a parameter. Eg.: 
```(print "2 + 2 = " (+ 2 2) "\n" "x! = " (factorial x) "\n" "x = " x "\n")```
#### Comparison operators
Comparing numbers has also been simplified. No more ```(a == b && a == c && c == d)```. No more ```(a < b && b < c)```. Chain them together. Type ```(equals a b c d)``` or ```(< a b c)``` and get the exact same result.

