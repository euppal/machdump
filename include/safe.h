// include/safe.h
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

#pragma once

#include <stdio.h>
#include <stdlib.h>

enum failure {
    VirtualMemoryExhausted,
    FailedToOpenFile,
    FailedToReadFile,
    FailedToCloseFile
};

void set_failure_handler(void (*handler)(enum failure));

void* xmalloc(size_t n);
#define xfree free

FILE* xfopen(const char* filename, const char* mode);
unsigned long xfread(void* buffer, size_t size, size_t count, FILE* stream);
int xfclose(FILE* file);

#define xfseek fseek
#define xftell ftell
#define xrewind rewind
