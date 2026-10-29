#include <jni.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <cstring>
#include "sdk.hpp"
#include "gui.hpp"

void* obsidian_worker(void*) {
    prctl(PR_SET_NAME, "system_fx_d", 0, 0, 0);

    uintptr_t il2cpp_base = 0;
    while (il2cpp_base == 0) {
        FILE* fp = fopen("/proc/self/maps", "r");
        if (fp) {
            char line[512];
            while (fgets(line, sizeof(line), fp)) {
                if (strstr(line, "libil2cpp.so")) {
                    il2cpp_base = strtoull(line, nullptr, 16);
                    break;
                }
            }
            fclose(fp);
        }
        usleep(200000);
    }

    while (true) {
        usleep(10000);
    }

    return nullptr;
}

__attribute__((constructor))
void obsidian_entry() {
    pthread_t thread;
    pthread_create(&thread, nullptr, obsidian_worker, nullptr);
    pthread_detach(thread);
}
