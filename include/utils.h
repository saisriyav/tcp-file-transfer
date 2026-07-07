#ifndef UTILS_H
#define UTILS_H

void log_info(const char *message);
void log_error(const char *message);
void print_progress(long transferred, long total);
double get_time_in_seconds(void);
void print_transfer_stats(long file_size, double elapsed_time);

#endif
