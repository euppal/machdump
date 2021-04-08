// src/dump.c
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

#include "dump.h"
#include "safe.h"
#include <mach-o/loader.h>

#define S(...) struct __VA_ARGS__
#define START_READ() size_t __CUR = 0
#define READ(n) buffer + __CUR; do { \
    __CUR += (n); \
    if (__CUR >= length) return; \
} while (0)
#define CONSUME(n) __CUR += (n)

#define local static inline

local void dump_header(S(mach_header_64)* header) {
    printf("│ Header\n");
    printf("└─┐ Magic: 0x%08x\n", header->magic);
    
    printf("  │ CPU Type: 0x%08x:", header->cputype);
    if (header->cputype == CPU_TYPE_ANY) {
        printf(" CPU_TYPE_ANY");
    } else if (header->cputype == CPU_TYPE_X86_64) {
        printf(" CPU_TYPE_X86_64");
    } else if (header->cputype == CPU_TYPE_POWERPC64) {
        printf(" CPU_TYPE_POWERPC64");
    }
    fputc('\n', stdout);
    
    printf("  │ CPU Subtype: 0x%08x:", header->cpusubtype);
    if (header->cpusubtype == CPU_SUBTYPE_POWERPC_ALL) {
        printf(" CPU_SUBTYPE_POWERPC_ALL");
    } else if (header->cpusubtype == CPU_SUBTYPE_X86_64_ALL) {
        printf(" CPU_SUBTYPE_X86_64_ALL");
    }
    fputc('\n', stdout);
    
    printf("  │ File Type: 0x%08x: ", header->filetype);
    if (header->filetype == MH_OBJECT) {
        printf("MH_OBJECT: Intermediate Object File\n");
    } else if (header->filetype == MH_EXECUTE) {
        printf("MH_EXECUTE: Standard Executable Program\n");
    } else if (header->filetype == MH_BUNDLE) {
        printf("MH_BUNDLE: Runtime Bundle\n");
    } else if (header->filetype == MH_DYLIB) {
        printf("MH_DYLIB: Dynamic Shared Library\n");
    } else if (header->filetype == MH_PRELOAD) {
        printf("MH_PRELOAD: Special Executable Program\n");
    } else if (header->filetype == MH_CORE) {
        printf("MH_CORE: Core File\n");
    } else if (header->filetype & MH_DYLINKER) {
        printf("MH_DYLINKER: Dynamic Linker Shared Library\n");
    } else if (header->filetype & MH_DSYM) {
        printf("MH_DSYM: Symbol Information File\n");
    }
    
    printf("  │ Number of load commands: %u\n", header->ncmds);
    printf("  │ Size of load commands: %u byte(s)\n", header->sizeofcmds);
    
    printf("  │ Flags: 0x%08x:", header->flags);
    if (header->flags == MH_NOUNDEFS) {
        printf(" MH_NOUNDEFS");
    }
    if (header->flags & MH_INCRLINK) {
        printf(" MH_INCRLINK");
    }
    if (header->flags & MH_DYLDLINK) {
        printf(" MH_DYLDLINK");
    }
    if (header->flags & MH_TWOLEVEL) {
        printf(" MH_TWOLEVEL");
    }
    if (header->flags & MH_BINDATLOAD) {
        printf(" MH_BINDATLOAD");
    }
    if (header->flags & MH_PREBOUND) {
        printf(" MH_PREBOUND");
    }
    if (header->flags & MH_PREBINDABLE) {
        printf(" MH_PREBINDABLE\n");
    }
    if (header->flags & MH_NOFIXPREBINDING) {
        printf(" MH_NOFIXPREBINDING");
    }
    if (header->flags & MH_ALLMODSBOUND) {
        printf(" MH_ALLMODSBOUND");
    }
    if (header->flags & MH_CANONICAL) {
        printf(" MH_CANONICAL");
    }
    if (header->flags & MH_SPLIT_SEGS) {
        printf(" MH_SPLIT_SEGS");
    }
    if (header->flags & MH_FORCE_FLAT) {
        printf(" MH_FORCE_FLAT");
    }
    if (header->flags & MH_SUBSECTIONS_VIA_SYMBOLS) {
        printf(" MH_SUBSECTIONS_VIA_SYMBOLS");
    }
    if (header->flags & MH_NOMULTIDEFS) {
        printf(" MH_NOMULTIDEFS");
    }
    if (header->flags == 0) {
        printf(" None");
    }
    fputc('\n', stdout);
}

local void dump_section_64(S(section_64*) sec64) {
    printf("  │ Section 64: struct section_64\n");
    printf("  └─┐ Section Name: %.16s\n", sec64->sectname);
    printf("    │ Segment Name: %.16s\n", sec64->segname);
    printf("    │ Virtual Memory Address: 0x%016llx\n", sec64->addr);
    printf("    │ Virtual Memory Size: 0x%016llx\n", sec64->size);
    printf("    │ File Offset: 0x%08x\n", sec64->offset);
    printf("    │ Section Alignment: 2**%u\n", sec64->align);
    printf("    │ File offset of first relocation entry: 0x%08x\n",
           sec64->reloff);
    printf("    │ Number of first relocation entries: %u\n", sec64->nreloc);
    printf("    │ Flags: %08x\n", sec64->flags);
    printf("  ┌─┘\n");
}

local void dump_segment_64(S(segment_command_64*) seg64) {
    printf("  │ Command Size: %u byte(s)\n", seg64->cmdsize);
    printf("  │ Segment Name: %.16s\n", seg64->segname);
    printf("  │ Virtual Memory Address: 0x%016llx\n", seg64->vmaddr);
    printf("  │ Virtual Memory Size: 0x%016llx\n", seg64->vmsize);
    printf("  │ File Offset: 0x%016llx\n", seg64->fileoff);
    printf("  │ File Size: 0x%016llx\n", seg64->filesize);
    printf("  │ Maximum Virtual Memory Protection: %08x\n", seg64->maxprot);
    printf("  │ Initial Virtual Memory Protection: %08x\n", seg64->initprot);
    printf("  │ Number of sections: %u\n", seg64->nsects);
    
    printf("  │ Flags: %08x:", seg64->flags);
    if (seg64->flags & SG_HIGHVM) {
        printf(" SG_HIGHVM");
    }
    if (seg64->flags & SG_NORELOC) {
        printf(" SG_NORELOC");
    }
    if (seg64->flags == 0) {
        printf(" None");
    }
    fputc('\n', stdout);
    
    char* sections = (void*)(seg64 + 1);
    for (uint32_t i = 0; i < seg64->nsects; i++) {
        S(section_64*) section = (void*)sections;
        sections += section->size;
        dump_section_64(section);
    }
}

local void dump_load_command(S(load_command*) load_command) {
    printf("┌─┘\n");
    printf("│ Load Command\n");
    printf("└─┐ Command Type: %08x: ", load_command->cmd);
    
    if (load_command->cmd == LC_UUID) {
        printf("LC_UUID: struct uuid_command\n");
    } else if (load_command->cmd == LC_SEGMENT) {
        printf("LC_SEGMENT: struct segment_command\n");
    } else if (load_command->cmd == LC_SEGMENT_64) {
        printf("LC_SEGMENT_64: struct segment_command_64\n");
        dump_segment_64((S(segment_command_64*))load_command);
    } else if (load_command->cmd == LC_SYMTAB) {
        printf("LC_SYMTAB: struct symtab_command\n");
    } else if (load_command->cmd == LC_DYSYMTAB) {
        printf("LC_DYSYMTAB: struct dysymtab_command\n");
    } else if (load_command->cmd == LC_THREAD) {
        printf("LC_THREAD: struct thread_command\n");
    } else if (load_command->cmd == LC_UNIXTHREAD) {
        printf("LC_UNIXTHREAD: struct thread_command\n");
    } else if (load_command->cmd == LC_LOAD_DYLIB) {
        printf("LC_LOAD_DYLIB: struct dylib_command\n");
    } else if (load_command->cmd == LC_ID_DYLIB) {
        printf("LC_ID_DYLIB: struct dylib_command\n");
    } else if (load_command->cmd == LC_PREBOUND_DYLIB) {
        printf("LC_PREBOUND_DYLIB: struct prebound_dylib_command\n");
    } else if (load_command->cmd == LC_LOAD_DYLINKER) {
        printf("LC_LOAD_DYLINKER: struct dylinker_command\n");
    } else if (load_command->cmd == LC_ID_DYLINKER) {
        printf("LC_ID_DYLINKER: struct dylinker_command\n");
    } else if (load_command->cmd == LC_ROUTINES) {
        printf("LC_ROUTINES: struct routines_command\n");
    } else if (load_command->cmd == LC_ROUTINES_64) {
        printf("LC_ROUTINES_64: struct routines_command_64\n");
    } else if (load_command->cmd == LC_TWOLEVEL_HINTS) {
        printf("LC_TWOLEVEL_HINTS: struct twolevel_hints_command\n");
    } else if (load_command->cmd == LC_SUB_FRAMEWORK) {
        printf("LC_SUB_FRAMEWORK: struct sub_framework_command\n");
    } else if (load_command->cmd == LC_SUB_UMBRELLA) {
        printf("LC_SUB_UMBRELLA: struct sub_umbrella_command\n");
    } else if (load_command->cmd == LC_SUB_LIBRARY) {
        printf("LC_SUB_LIBRARY: struct sub_library_command\n");
    } else if (load_command->cmd == LC_SUB_CLIENT) {
        printf("LC_SUB_CLIENT: struct sub_client_command\n");
    }
    #ifdef LC_SYMSEG
    else if (load_command->cmd == LC_SYMSEG) {
        printf("LC_SYMSEG: struct symseg_command\n");
    }
    #endif
    else {
        printf("Unknown\n");
    }
}

void mach_dump(void* buffer, const size_t length) {
    START_READ();
    
    S(mach_header_64*) header = READ(sizeof(*header));
    dump_header(header);
    
    for (uint32_t i = 0; i < header->ncmds; i++) {
        S(load_command*) load_command = READ(sizeof(*load_command));
        CONSUME(load_command->cmdsize - sizeof(*load_command));
        dump_load_command(load_command);
    }
}
