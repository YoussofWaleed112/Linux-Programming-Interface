int pwd_main() {
    char *buf = getcwd(NULL, 0);
    if (buf == NULL) {
        perror("pwd");
        return 1;
    }
    printf("%s\n", buf);
    free(buf);
    return 0;
}



int echo_main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (printf("%s", argv[i]) < 0) {
            return 1;
        }
        if (i < argc - 1) {
            if (printf(" ") < 0) {
                return 1;
            }
        }
    }
    if (printf("\n") < 0) {
        return 1;
    }
    return 0;
}






#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int cp_main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: cp <source> <destination>\n");
        return 1;
    }

    // Open source file for reading
    int src_fd = open(argv[1], O_RDONLY);
    if (src_fd < 0) {
        perror("cp: cannot open source");
        return 1;
    }

    // Get source file permissions to replicate them on destination
    struct stat src_stat;
    if (fstat(src_fd, &src_stat) < 0) {
        perror("cp: cannot stat source");
        close(src_fd);
        return 1;
    }

    // Open destination file for writing (create or truncate), same permissions as source
    int dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, src_stat.st_mode);
    if (dst_fd < 0) {
        perror("cp: cannot open destination");
        close(src_fd);
        return 1;
    }

    // Copy in chunks
    char buf[4096];
    ssize_t bytes_read, bytes_written;

    while ((bytes_read = read(src_fd, buf, sizeof(buf))) > 0) {
        char *write_ptr = buf;
        bytes_written = 0;

        // Handle partial writes
        while (bytes_written < bytes_read) {
            ssize_t ret = write(dst_fd, write_ptr, bytes_read - bytes_written);
            if (ret < 0) {
                perror("cp: write error");
                close(src_fd);
                close(dst_fd);
                return 1;
            }
            bytes_written += ret;
            write_ptr += ret;
        }
    }

    if (bytes_read < 0) {
        perror("cp: read error");
        close(src_fd);
        close(dst_fd);
        return 1;
    }

    close(src_fd);
    close(dst_fd);
    return 0;
}





#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

int mv_main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: mv <source> <destination>\n");
        return 1;
    }

    // Try rename() first — works instantly if src and dst are on the same filesystem
    if (rename(argv[1], argv[2]) == 0) {
        return 0;
    }

    // If rename() failed for a reason other than cross-device, report and exit
    if (errno != EXDEV) {
        perror("mv");
        return 1;
    }

    // Cross-device move: fall back to copy + delete
    // Get source permissions
    struct stat src_stat;
    if (stat(argv[1], &src_stat) < 0) {
        perror("mv: cannot stat source");
        return 1;
    }

    // Open source for reading
    int src_fd = open(argv[1], O_RDONLY);
    if (src_fd < 0) {
        perror("mv: cannot open source");
        return 1;
    }

    // Open destination for writing
    int dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, src_stat.st_mode);
    if (dst_fd < 0) {
        perror("mv: cannot open destination");
        close(src_fd);
        return 1;
    }

    // Copy in chunks with partial write handling
    char buf[4096];
    ssize_t bytes_read, bytes_written;

    while ((bytes_read = read(src_fd, buf, sizeof(buf))) > 0) {
        char *write_ptr = buf;
        bytes_written = 0;

        while (bytes_written < bytes_read) {
            ssize_t ret = write(dst_fd, write_ptr, bytes_read - bytes_written);
            if (ret < 0) {
                perror("mv: write error");
                close(src_fd);
                close(dst_fd);
                unlink(argv[2]);  // Clean up partial destination file
                return 1;
            }
            bytes_written += ret;
            write_ptr += ret;
        }
    }

    if (bytes_read < 0) {
        perror("mv: read error");
        close(src_fd);
        close(dst_fd);
        unlink(argv[2]);  // Clean up partial destination file
        return 1;
    }

    close(src_fd);
    close(dst_fd);

    // Delete the source file after successful copy
    if (unlink(argv[1]) < 0) {
        perror("mv: cannot remove source");
        return 1;
    }

    return 0;
}
