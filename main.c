// main.c
// Copyright (C) 2021 Ethan Uppal
//
// machdump is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// machdump is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with machdump. If not, see <https://www.gnu.org/licenses/>.

#include <stdio.h>
#include <string.h>
#include "include/dump.h"
#include "include/safe.h"

void driver(const char* filename) {
    FILE* file = xfopen(filename, "r");
    xfseek(file, 0, SEEK_END);
    const size_t length = xftell(file);
    xrewind(file);
    
    void* buffer = xmalloc(length);
    xfread(buffer, sizeof(char), length, file);
    
    xfclose(file);
    
    mach_dump(buffer, length);
    
    xfree(buffer);
}

static void print_help(const char* argv[]) {
    printf("Usage: %s [--help|--version]\n"
           "   or: %s [FILE...]\n", argv[0],
           argv[0]);
}

static void print_version(void) {
    printf("Copyright (C) 2021 Ethan Uppal\n"
           "License GPLv3+: GNU GPL version 3 or later "
           "<https://gnu.org/licenses/gpl.html>\n"
           "This is free software: you are free to change and redistribute "
           "it.\n"
           "There is NO WARRANTY, to the extent permitted by law.\n");
}

int main(int argc, const char* argv[]) {
    if (argc == 1) {
        print_help(argv);
    } else if (argc == 2) {
        if (strncmp(argv[1], "--help", 7) == 0) {
            print_help(argv);
        } else if (strncmp(argv[1], "--version", 10) == 0) {
            print_version();
        } else {
            for (int i = 1; i < argc; i++) {
                driver(argv[i]);
            }
        }
    }
}
