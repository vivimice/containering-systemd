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
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

static int stdout_dup_fd = 0;
static int (*main_proxy_target)(int, char **, char **);

/**
 * Main() function hook
 */
static int main_proxy(int argc, char **argv, char **envp) {
    // Duplicate stdout 
    stdout_dup_fd = dup(STDOUT_FILENO);
    if (stdout_dup_fd == -1) {
        perror("Journalagent failed to duplicate stdout fd");
        exit(-1);
    }

    // Save the file descriptor number to environment variable
    // So service unit can refer it by using PassEnvironment= in its .service file
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", stdout_dup_fd);
    if (setenv("SYSTEMD_STDOUT_FILENO", buf, 1) == -1) {
        perror("Journalagent failed to save duplicated stdout fd to environment variable");
        exit(-1);
    }

    printf("Journalagent interception established. STDOUT duplicated as fd=%d\n", stdout_dup_fd);
    
    // Call the actual main() function of systemd
    return main_proxy_target(argc, argv, envp);
}

/**
 * Function that hooks before main() function
 */
int __libc_start_main(
    int (*main)(int, char **, char **),
    int argc,
    char **argv,
    int (*init)(int, char **, char **),
    void (*fini)(void),
    void (*rtld_fini)(void),
    void *stack_end
) {

    // get libc function entries
    typeof(&__libc_start_main) libc_start_main = dlsym(RTLD_NEXT, "__libc_start_main");
    // call our delegated main function 
    main_proxy_target = main;
    return libc_start_main(main_proxy, argc, argv, init, fini, rtld_fini, stack_end);
}

/**
 * Hook for libc wrapped close() syscall.
 */
int close(int fd) {
    // Prevent closing duplicated stdout fd
    if (stdout_dup_fd != 0 && fd == stdout_dup_fd) {
        // pretending that close() syscall succeeded
        return 0;
    }

    // Otherwise, pass it to the actual close() call
    static int (*libc_close)(int fd) = NULL;
    if (libc_close == NULL) {
        libc_close = dlsym(RTLD_NEXT, "close");
    }
    return libc_close(fd);
}
