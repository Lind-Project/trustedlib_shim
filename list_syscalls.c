#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // Open the unistd_64.h file which contains syscall numbers
    file = fopen("/usr/include/x86_64-linux-gnu/asm/unistd_64.h", "r");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    printf("Syscall Number\tSyscall Name\n");
    printf("-------------\t------------\n");

    // Read the file line by line
    while ((read = getline(&line, &len, file)) != -1) {
        // Look for lines defining syscall numbers
        if (strstr(line, "#define __NR_") != NULL) {
            char syscall_name[256];
            int syscall_number;

            // Parse the line to extract syscall name and number
            sscanf(line, "#define __NR_%s %d", syscall_name, &syscall_number);

            // Print the syscall number and name
            printf("%-13d\t%s\n", syscall_number, syscall_name);
        }
    }

    free(line);
    fclose(file);
    return 0;
}
