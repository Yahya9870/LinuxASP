#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ftw.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>

int search_successful = 0;
char *filename; // Declaring filename here

// Function to search for a file and display its absolute path
int search(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F) {
        char *file = strrchr(fpath, '/');
        if (file && strcmp(++file, filename) == 0) {
            printf("%s\n", fpath);
            search_successful = 1;
        }
    }
    return 0;
}

// Function to copy or move a file to the destination directory
void copy_or_move_file(char *source, char *destination, int move) {
    char command[PATH_MAX + PATH_MAX + 15];
    if (!move) {
        sprintf(command, "cp %s %s", source, destination);
    } else {
        sprintf(command, "mv %s %s", source, destination);
    }
    printf("Executing command: %s\n", command);
    if (system(command) != 0) {
        fprintf(stderr, "Failed to %s file\n", move ? "move" : "copy");
        exit(EXIT_FAILURE);
    }
}

// Function to create a tar file of files with a given extension in the specified directory
void create_tar_file(char *root_dir, char *storage_dir, char *extension) {
    char command[PATH_MAX + 100];
    sprintf(command, "find %s -type f -name '*.%s' -print | tar -cf %s/a1.tar -T -", root_dir, extension, storage_dir);
    printf("Executing command: %s\n", command);
    if (system(command) != 0) {
        fprintf(stderr, "Failed to create tar file\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 5) {
        fprintf(stderr, "Usage: %s [root_dir] filename\n", argv[0]);
        fprintf(stderr, "       %s [root_dir] [storage_dir] [options] filename\n", argv[0]);
        fprintf(stderr, "       %s [root_dir] [storage_dir] extension\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc == 3) {
        char *root_dir = argv[1];
        filename = argv[2]; // Setting filename here
        nftw(root_dir, search, 20, FTW_PHYS);
        if (!search_successful) {
            printf("Search Unsuccessful\n");
        }
    } else if (argc == 5) {
        char *root_dir = argv[1];
        char *storage_dir = argv[2];
        filename = argv[4]; // Setting filename here
        if (strcmp(argv[3], "-cp") == 0 || strcmp(argv[3], "-mv") == 0) {
            nftw(root_dir, search, 20, FTW_PHYS);
            if (search_successful) {
                copy_or_move_file(argv[4], storage_dir, strcmp(argv[3], "-mv") == 0);
            } else {
                printf("Search Unsuccessful\n");
            }
        } else {
            fprintf(stderr, "Invalid option: %s\n", argv[3]);
            exit(EXIT_FAILURE);
        }
    } else if (argc == 4) {
        char *root_dir = argv[1];
        char *storage_dir = argv[2];
        char *extension = argv[3];
        create_tar_file(root_dir, storage_dir, extension);
    }

    return 0;
}
