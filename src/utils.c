#include <stdio.h>
#include <sys/time.h>
#include "utils.h"

void log_info(const char *message) {
    printf("[INFO] %s\n", message);
}

void log_error(const char *message) {
    perror(message);
}

void print_progress(long transferred, long total) {
    if (total <= 0) {
        return;
    }

    int percent = (int)((transferred * 100) / total);
    printf("\rProgress: %d%%", percent);
    fflush(stdout);
}

double get_time_in_seconds(void) {
    struct timeval time_value;
    gettimeofday(&time_value, NULL);

    return time_value.tv_sec + (time_value.tv_usec / 1000000.0);
}

void print_transfer_stats(long file_size, double elapsed_time) {
    double file_size_mb = file_size / (1024.0 * 1024.0);
    double speed_mbps = 0.0;

    if (elapsed_time > 0) {
        speed_mbps = file_size_mb / elapsed_time;
    }

    printf("[INFO] Transfer Time : %.4f seconds\n", elapsed_time);
    printf("[INFO] File Size     : %.4f MB\n", file_size_mb);
    printf("[INFO] Speed         : %.4f MB/s\n", speed_mbps);
}
