#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <capstone/capstone.h>
#include <mach-o/loader.h>

using namespace std;

enum ScanType { STRING_XREF, HEX_PATTERN, VTABLE_INDEX };

struct OffsetTask {
    string name;
    ScanType type;
    string query;
    int index; 
};

uint8_t* g_Data = nullptr;
size_t g_Size = 0;
uint64_t g_TextVmAddr = 0, g_TextFileOff = 0, g_TextSize = 0;
map<string, uint64_t> g_FoundOffsets;

vector<uint8_t> ParseHex(string hex) {
    vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.length(); i++) {
        if (hex[i] == ' ') continue;
        if (i + 1 < hex.length()) {
            bytes.push_back((uint8_t)strtol(hex.substr(i, 2).c_str(), nullptr, 16));
            i++;
        }
    }
    return bytes;
}

uint64_t FileToVm(uint64_t fileOff, mach_header_64* header) {
    uint8_t* cmd = (uint8_t*)(header + 1);
    for (uint32_t i = 0; i < header->ncmds; i++, cmd += ((load_command*)cmd)->cmdsize) {
        auto* lc = (load_command*)cmd;
        if (lc->cmd == LC_SEGMENT_64) {
            auto* seg = (segment_command_64*)lc;
            if (fileOff >= seg->fileoff && fileOff < (seg->fileoff + seg->filesize))
                return seg->vmaddr + (fileOff - seg->fileoff);
        }
    }
    return 0;
}

bool SetupSegments(mach_header_64* header) {
    uint8_t* cmd = (uint8_t*)(header + 1);
    for (uint32_t i = 0; i < header->ncmds; i++, cmd += ((load_command*)cmd)->cmdsize) {
        auto* lc = (load_command*)cmd;
        if (lc->cmd == LC_SEGMENT_64) {
            auto* seg = (segment_command_64*)lc;
            if (string(seg->segname) == "__TEXT") {
                g_TextVmAddr = seg->vmaddr;
                g_TextFileOff = seg->fileoff;
                g_TextSize = seg->filesize;
                return true;
            }
        }
    }
    return false;
}

void RunScanner(mach_header_64* header, csh cs, const vector<OffsetTask>& tasks) {
    uint8_t* textStart = g_Data + g_TextFileOff;

    for (const auto& task : tasks) {
        uint64_t foundAddr = 0;

        if (task.type == STRING_XREF) {
            void* strLoc = memmem(g_Data, g_Size, task.query.c_str(), task.query.length());
            if (strLoc) {
                uint64_t strVm = FileToVm((uint8_t*)strLoc - g_Data, header);
                for (size_t i = 0; i < g_TextSize - 7; i++) {
                    if (textStart[i] == 0x48 && textStart[i+1] == 0x8D) {
                        int32_t disp = *(int32_t*)(textStart + i + 3);
                        if ((g_TextVmAddr + i + 7) + disp == strVm) {
                            cs_insn* insn;
                            size_t count = cs_disasm(cs, textStart + i, 500, g_TextVmAddr + i, 0, &insn);
                            int callsFound = 0;
                            for (size_t j = 0; j < count; j++) {
                                if (insn[j].id == X86_INS_CALL) {
                                    if (++callsFound == task.index) {
                                        foundAddr = insn[j].detail->x86.operands[0].imm;
                                        break;
                                    }
                                }
                            }
                            cs_free(insn, count);
                            break;
                        }
                    }
                }
            }
        } 
        else if (task.type == HEX_PATTERN) {
            vector<uint8_t> pattern = ParseHex(task.query);
            void* patLoc = memmem(textStart, g_TextSize, pattern.data(), pattern.size());
            if (patLoc) foundAddr = g_TextVmAddr + ((uint8_t*)patLoc - textStart);
        }
        else if (task.type == VTABLE_INDEX) {
            if (g_FoundOffsets.count(task.query)) {
                uint64_t anchorVm = g_FoundOffsets[task.query];
                void* vtableEntry = memmem(g_Data, g_Size, &anchorVm, sizeof(uint64_t));
                if (vtableEntry) {
                    uint64_t* table = (uint64_t*)((uint8_t*)vtableEntry + (task.index * 8));
                    foundAddr = *table;
                }
            }
        }

        if (foundAddr != 0) {
            g_FoundOffsets[task.name] = foundAddr;
            printf("[+] %-25s | Offset: 0x%-10llx | VM: 0x%llx\n", 
                   task.name.c_str(), (foundAddr - g_TextVmAddr), foundAddr);
        } else {
            printf("[-] %-25s | NOT FOUND\n", task.name.c_str());
        }
    }
}

int main(int argc, char** argv) {
    string targetPath = "/Applications/Roblox.app/Contents/MacOS/RobloxPlayer";
    
    if (argc >= 2) {
        targetPath = argv[1];
    }

    int fd = open(targetPath.c_str(), O_RDONLY);
    if (fd < 0) {
        printf("Error: Could not open RobloxPlayer at %s\n", targetPath.c_str());
        printf("Make sure Roblox is installed in your Applications folder.\n");
        return 1;
    }

    struct stat st;
    fstat(fd, &st);
    g_Size = st.st_size;
    g_Data = (uint8_t*)mmap(nullptr, g_Size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    mach_header_64* header = (mach_header_64*)g_Data;
    if (header->magic != MH_MAGIC_64) {
        printf("Error: Target is not a valid 64-bit Mach-O binary.\n");
        return 1;
    }

    if (!SetupSegments(header)) {
        printf("Error: Failed to locate __TEXT segment.\n");
        return 1;
    }

    csh handle;
    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) return 1;
    cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);

    vector<OffsetTask> tasks = {
        { "Print", STRING_XREF, "Current identity is %d", 1 }
    };

    RunScanner(header, handle, tasks);

    cs_close(&handle);
    munmap(g_Data, g_Size);
    return 0;
}
