// RUN: rm -rf %t
// RUN: split-file %s %t
// RUN: cd %t
//
// RUN: %clang_cc1 -std=c++20 -emit-module-interface %t/a.cppm -o %t/a.pcm
// RUN: %clang_cc1 -std=c++20 -emit-module-interface %t/b.cppm -o %t/b.pcm
// RUN: %clang_cc1 -std=c++20 -fsyntax-only -verify %t/c.cpp \
// RUN:     -fprebuilt-module-path=%t
// RUN: %clang_cc1 -std=c++20 -fsyntax-only -verify %t/d.cpp \
// RUN:     -fprebuilt-module-path=%t
//
// RUN: %clang_cc1 -std=c++20 -emit-module-interface %t/h.cppm \
// RUN:     -fprebuilt-module-path=%t -o %t/h.pcm
// RUN: %clang_cc1 -std=c++20 -fsyntax-only -verify %t/j.cpp \
// RUN:     -fprebuilt-module-path=%t

//--- a.cppm
export module a;
export void foo() {

}

//--- b.cppm
export module b;
void bar();
export void foo() {
    bar();
}

//--- c.cpp
// expected-no-diagnostics
// Since we will load all the declaration lazily, we won't be able to find
// the ODR violation here.
import a;
import b;

//--- d.cpp
import a;
import b;
// Test that we can still check the odr violation if we call the function
// actually.
void use() {
    foo(); // expected-error@* {{'foo' has different definitions in different modules;}}
           // expected-note@* {{but in 'a' found a different body}}
}

// expected-error@a.cppm:* {{declaration 'foo' attached to named module 'a' cannot be attached to other modules}}
// expected-note@b.cppm:* {{}}

//--- h.cppm
export module h;
export import a;
export import b;

//--- j.cpp
import h;
void use() {
    foo(); // expected-error@* {{'foo' has different definitions in different modules;}}
           // expected-note@* {{but in 'a' found a different body}}
}

// expected-error@a.cppm:* {{declaration 'foo' attached to named module 'a' cannot be attached to other modules}}
// expected-note@b.cppm:* {{}}
