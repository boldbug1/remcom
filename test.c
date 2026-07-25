#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

// Function to clean and normalize a input string
void sanitize_text(const char *input, char *output) {
    int read_idx = 0;   // Source string index
    int write_idx = 0;  // Target buffer index

    // Process characters until reaching null terminator
    while (input[read_idx] != '\0') {
        char current = input[read_idx]; // Read current byte

        // Keep numbers and letters, discard punctuation
        if (isalnum((unsigned char)current)) {
            // Convert uppercase letters to lowercase
            output[write_idx] = (char)tolower((unsigned char)current);
            write_idx++; // Advance write position
        }

        read_idx++; // Move to next input byte
    }

    // Append null terminator to complete string
    output[write_idx] = '\0';
}

// Helper to print test results side-by-side
void run_test(const char *test_case) {
    char result[100]; // Allocation buffer for output

    // Pass target string into sanitizer
    sanitize_text(test_case, result);
/*
This
is 
a multi
line fstgwgftwsefaws\\\23u123i10i32@@#$%^&*()
comment
*/
    // Format output stream
    printf("Original : %s\n", test_case);
    printf("Sanitized: %s\n\n", result);
}

int main(void) {
    // Test case 1: Mixed casing and spaces
    run_test("Hello, World! 123");

    // Test case 2: Punctuation heavy string
    run_test("C-Language -- #Filtering_Comments!");

    // Exit successfully
    return 0;
}