#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024 // Define the maximum line length

// Structure to store a line
typedef struct LineNode {
    char *line;                // Pointer to store the line of text
    struct LineNode *next;    // Pointer to the next node (link to the next line)
} LineNode;

// Function to add a line to the linked list
LineNode* add_line(LineNode *head, char *line) {
    LineNode *new_node = (LineNode *)malloc(sizeof(LineNode)); // Allocate memory for a new node
    if (new_node == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(1); // Exit if there is not enough memory
    }
    new_node->line = strdup(line); // Copy the line into the new node
    if (new_node->line == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(1); // Exit if there is not enough memory
    }
    new_node->next = head; // The new node points to the previous head (which was the previous head)
    return new_node; // Return the new node which is now the head of the list
}

// Function to free the memory of the linked list
void free_lines(LineNode *head) {
    LineNode *current = head; // Start from the head of the list
    while (current != NULL) {
        LineNode *temp = current;  // Save the current node to free it later
        current = current->next;   // Move to the next node
        free(temp->line);          // Free the memory of the line
        free(temp);                // Free the memory of the node
    }
}

// Main function
int main(int argc, char *argv[]) {
    FILE *input_file = stdin;  // Default input is the terminal
    FILE *output_file = stdout; // Default output is the terminal
    LineNode *line_list = NULL; // Initialize the list of lines as empty
    char buffer[MAX_LINE_LENGTH]; // Buffer to temporarily store lines read

    // File names for comparison
    char *input_filename = NULL;
    char *output_filename = NULL;

    // Argument handling
    if (argc > 3) { // If more than two arguments are passed, there is an error
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    if (argc >= 2) { // If an input file is provided
        input_filename = argv[1];
        input_file = fopen(input_filename, "r"); // Open the input file
        if (input_file == NULL) { // If the file cannot be opened, show an error
            fprintf(stderr, "reverse: cannot open file '%s'\n", input_filename);
            exit(1);
        }
    }

    if (argc == 3) { // If an output file is also provided
        output_filename = argv[2];
        output_file = fopen(output_filename, "w"); // Open the output file
        if (output_file == NULL) { // If the file cannot be opened, show an error
            fprintf(stderr, "reverse: cannot open file '%s'\n", output_filename);
            exit(1);
        }

        // Check if input and output files are the same by comparing filenames
        if (strcmp(input_filename, output_filename) == 0) {
            fprintf(stderr, "reverse: input and output file must differ\n");
            exit(1);
        }
    }

    // Read the input file line by line and add to the list
    while (1) {
        // Read a line in chunks if necessary
        char *full_line = NULL;
        size_t full_length = 0;
        size_t current_length = 0;
        
        // Read a line in chunks
        while (fgets(buffer, sizeof(buffer), input_file) != NULL) {
            size_t buffer_length = strlen(buffer);
            char *new_line = (char *)realloc(full_line, full_length + buffer_length + 1);
            if (new_line == NULL) {
                fprintf(stderr, "malloc failed\n");
                exit(1);
            }
            full_line = new_line;
            memcpy(full_line + full_length, buffer, buffer_length + 1);
            full_length += buffer_length;

            // If a newline is found, end reading the line
            if (buffer[buffer_length - 1] == '\n') break;
        }

        // If we are done reading, exit the loop
        if (full_line == NULL && feof(input_file)) break;

        // Add the complete line to the list
        if (full_line != NULL) {
            line_list = add_line(line_list, full_line);
            free(full_line);
        }
    }

    // Print the lines in reverse order
    LineNode *current = line_list;
    while (current != NULL) {
        fprintf(output_file, "%s", current->line); // Write each line to the output file
        current = current->next; // Move to the next node
    }

    // Clean up and free the memory used
    free_lines(line_list); // Free the memory of the lines
    if (input_file != stdin) fclose(input_file); // Close the input file if it's not stdin
    if (output_file != stdout) fclose(output_file); // Close the output file if it's not stdout

    return 0; // End the program
}
