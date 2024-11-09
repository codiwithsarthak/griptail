#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <pwd.h>
#include <sys/statvfs.h>
#include <time.h>
#include <ctype.h>

#define COLOR_RESET   "\x1b[0m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_WHITE   "\x1b[37m"
#define COLOR_GRAY    "\x1b[90m"

char* get_distro_name(void) {
    FILE* os_release = fopen("/etc/os-release", "r");
    static char distro_name[256] = "Unknown";

    if (os_release) {
        char line[256];
        while (fgets(line, sizeof(line), os_release)) {
            if (strncmp(line, "PRETTY_NAME=", 12) == 0) {
                char* name = strchr(line, '=') + 1;
                if (name[0] == '"') {
                    name++;
                    name[strlen(name)-2] = '\0';
                }
                strncpy(distro_name, name, sizeof(distro_name)-1);
                break;
            }
        }
        fclose(os_release);
    }
    return distro_name;
}

char* get_desktop_environment(void) {
    char* de = getenv("XDG_CURRENT_DESKTOP");
    return de ? de : "Unknown";
}

char* get_shell(void) {
    char* shell = getenv("SHELL");
    if (shell) {
        return strrchr(shell, '/') ? strrchr(shell, '/') + 1 : shell;
    }
    return "Unknown";
}

void format_uptime(long seconds, char* buffer) {
    int days = seconds / (24*3600);
    seconds = seconds % (24*3600);
    int hours = seconds / 3600;
    seconds = seconds % 3600;
    int minutes = seconds / 60;

    sprintf(buffer, "%d days, %d hours, %d minutes", days, hours, minutes);
}

void format_memory(unsigned long bytes, char* buffer) {
    double gb = bytes / (1024.0 * 1024.0 * 1024.0);
    sprintf(buffer, "%.1f GB", gb);
}

void get_system_info(void) {
    struct utsname sys_info;
    struct sysinfo si;
    struct passwd *pw = getpwuid(getuid());
    char hostname[256];
    char uptime_buffer[100];
    char total_mem[20], used_mem[20];

    uname(&sys_info);
    sysinfo(&si);
    gethostname(hostname, sizeof(hostname));

    format_uptime(si.uptime, uptime_buffer);
    format_memory(si.totalram, total_mem);
    format_memory(si.totalram - si.freeram, used_mem);

    printf("%s┌─────── Griptail System Info ────────┐%s\n", COLOR_BLUE, COLOR_RESET);
    printf("%s│%s %s%s@%s%s\n", COLOR_BLUE, COLOR_RESET,
           pw->pw_name, COLOR_BLUE, hostname, COLOR_RESET);
    printf("%s│%s OS: %s\n", COLOR_BLUE, COLOR_RESET, get_distro_name());
    printf("%s│%s Kernel: %s %s\n", COLOR_BLUE, COLOR_RESET,
           sys_info.sysname, sys_info.release);
    printf("%s│%s Uptime: %s\n", COLOR_BLUE, COLOR_RESET, uptime_buffer);
    printf("%s│%s Memory: %s / %s\n", COLOR_BLUE, COLOR_RESET, used_mem, total_mem);
    printf("%s│%s Shell: %s\n", COLOR_BLUE, COLOR_RESET, get_shell());
    printf("%s│%s DE: %s\n", COLOR_BLUE, COLOR_RESET, get_desktop_environment());
    printf("%s└─────────────────────────────────────┘%s\n", COLOR_BLUE, COLOR_RESET);
}

int main(void) {
    get_system_info();
    return 0;
}
