// Ferley Silva
// Carlos Zapata

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> // Required for using the stat structure

// Structure to store a line in a linked list
typedef struct LineNode {
    char *line;
    struct LineNode *next;
} LineNode;

// Function to add a line to the linked list
LineNode* add_line(LineNode *head, char *line) {
    LineNode *new_node = (LineNode *)malloc(sizeof(LineNode)); // Allocate memory for a new node
    if (new_node == NULL) { // Check if memory allocation failed
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    new_node->line = strdup(line); // Duplicate the string to store it in the node
    if (new_node->line == NULL) { // Check if string duplication failed
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    new_node->next = head; // Add the new node to the beginning of the list
    return new_node;
}

// Function to free the memory used by the linked list
void free_lines(LineNode *head) {
    LineNode *current = head;
    while (current != NULL) {
        LineNode *temp = current;
        current = current->next;
        free(temp->line); // Free the stored line
        free(temp); // Free the node itself
    }
}

// Function to read lines of arbitrary length
ssize_t read_line(char **lineptr, size_t *n, FILE *stream) {
    if (*lineptr == NULL || *n == 0) {
        *n = 128; // Initial buffer size
        *lineptr = malloc(*n); // Allocate memory for the buffer
        if (*lineptr == NULL) {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }
    }

    ssize_t num_chars = getline(lineptr, n, stream); // Read the line
    if (num_chars == -1) { // Error or end of file
        return -1;
    }

    return num_chars;
}

// Function to check if two files are the same (hardlinked)
int are_files_same(const char *file1, const char *file2) {
    struct stat stat1, stat2;

    // Get information for the first file
    if (stat(file1, &stat1) != 0) {
        exit(1);
    }

    // Get information for the second file
    if (stat(file2, &stat2) != 0) {
        exit(1);
    }

    // Compare device number and inode number
    return (stat1.st_dev == stat2.st_dev && stat1.st_ino == stat2.st_ino);
}

// Main function
int main(int argc, char *argv[]) {
    FILE *input_file = stdin; // Default input is stdin
    FILE *output_file = stdout; // Default output is stdout
    LineNode *lines = NULL; // Pointer to the linked list of lines
    char *buffer = NULL; // Buffer to store the current line
    size_t buffer_size = 0; // Size of the buffer

    // Argument handling
    if (argc > 3) { // Too many arguments
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    if (argc == 2) { // One argument: input file specified
        input_file = fopen(argv[1], "r"); // Open the input file
        if (input_file == NULL) { // Check if the file could not be opened
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
    }

    if (argc == 3) { // Two arguments: input and output files specified
        input_file = fopen(argv[1], "r"); // Open the input file
        output_file = fopen(argv[2], "w"); // Open the output file
        if (input_file == NULL) { // Check if input file could not be opened
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
        if (output_file == NULL) { // Check if output file could not be opened
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[2]);
            exit(1);
        }

        // Check if the input and output files are the same
        if (strcmp(argv[1], argv[2]) == 0 || are_files_same(argv[1], argv[2])) {
            fprintf(stderr, "reverse: input and output file must differ\n");
            exit(1);
        }
    }

    // Read the input file line by line and add to the linked list
    while (read_line(&buffer, &buffer_size, input_file) != -1) {
        lines = add_line(lines, buffer); // Add the line to the linked list
    }

    // Print the lines in reverse order
    LineNode *current = lines;
    while (current != NULL) {
        fprintf(output_file, "%s", current->line); // Print the line
        current = current->next;
    }

    // Clean up
    free_lines(lines); // Free the linked list
    free(buffer); // Free the buffer
    if (input_file != stdin) fclose(input_file); // Close input file if it was opened
    if (output_file != stdout) fclose(output_file); // Close output file if it was opened

    return 0;
}

