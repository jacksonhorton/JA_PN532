// Example usage for PN532 library by Jackson Horton.

#include "PN532.h"

// Initialize objects from the lib
PN532 pN532;

void setup() {
    // Call functions on initialized library objects that require hardware
    pN532.begin();
}

void loop() {
    // Use the library's initialized objects and functions
    pN532.process();
}
