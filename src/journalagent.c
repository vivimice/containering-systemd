// Copyright 2024 vivimice <vivimice@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    int systemd_pidfd = syscall(SYS_pidfd_open, 1, 0);
    if (systemd_pidfd == -1) {
        perror("Cannot get pidfd of systemd");
        exit(156);
    }

    char *target_fd_str = getenv("SYSTEMD_STDOUT_FILENO");
    if (target_fd_str == NULL) {
        fprintf(stderr, "Env not defined: SYSTEMD_STDOUT_FILENO\n");
        exit(156);
    }

    int target_fd = atoi(target_fd_str);
    if (target_fd <= 0) {
        fprintf(stderr, "Illegal SYSTEMD_STDOUT_FILENO: %s\n", target_fd_str);
        exit(156);
    }

    int systemd_stdout_fd = syscall(SYS_pidfd_getfd, systemd_pidfd, target_fd, 0);
    if (systemd_stdout_fd == -1) {
        char buf[256];
        snprintf(buf, sizeof(buf), "pidfd_getfd syscall on fd=%d failed", target_fd);
        perror(buf);
        exit(156);
    }

    dup2(systemd_stdout_fd, STDOUT_FILENO);
    dup2(systemd_stdout_fd, STDERR_FILENO);
    execvp("/usr/bin/journalctl", argv);
    perror("Failed execvp journalctl");
    exit(156);

    return 0;
}