#include "includes/utils.h"
#include <android/input.h>
#include <android/keycodes.h>

namespace utils{
    bool is_library_loaded(const char *lib) {
        char line[512] = {0};
        FILE *fp = fopen("/proc/self/maps", "rt");
        if (fp != NULL) {
            while (fgets(line, sizeof(line), fp)) {
                if (strstr(line, lib))
                    return true;
            }
            fclose(fp);
        }
        return false;
    }

    lib_info find_library(const char *module_name){
        lib_info library_info{};
        char line[512], mod_name[64];

        FILE *fp = fopen("/proc/self/maps", "rt");
        if (fp != NULL)
        {
            while (fgets(line, sizeof(line), fp))
            {
                if (strstr(line, module_name))
                {
                    sscanf(line, "%llx-%llx %*s %*ld %*s %*d %s",
                           (long long unsigned *)&library_info.start_address,
                           (long long unsigned *)&library_info.end_address, mod_name);

                    library_info.size = (uintptr_t)library_info.end_address - (uintptr_t)library_info.start_address;

                    if (library_info.name.empty())
                        library_info.name = std::string(mod_name);

                    break;
                }
            }
            fclose(fp);
        }
        return library_info;
    }

    uintptr_t get_absolute_address(const char* lib_name, uintptr_t relative_addr){
        return (reinterpret_cast<uintptr_t>(find_library(lib_name).start_address) + relative_addr);
    }

    uintptr_t find_pattern(uint8_t* start, const size_t length, const char* pattern) {
        const char* pat = pattern;
        uint8_t* first_match = 0;
        for (auto current_byte = start; current_byte < (start + length); ++current_byte) {
            if (*pat == '?' || *current_byte == strtoul(pat, NULL, 16)) {
                if (!first_match)
                    first_match = current_byte;
                if (!pat[2])
                    return (uintptr_t)first_match;
                pat += *(uint16_t*)pat == 16191 || *pat != '?' ? 3 : 2;
            }
            else if (first_match) {
                current_byte = first_match;
                pat = pattern;
                first_match = 0;
            }
        } return 0;
    }

    uintptr_t find_pattern_in_module(const char* lib_name, const char* pattern) {
        lib_info lib_info = find_library(lib_name);
        return find_pattern((uint8_t*)lib_info.start_address, lib_info.size, pattern);
    }

    ImGuiKey KeyCodeToImGuiKey(int32_t key_code) {
        switch (key_code) {
            case AKEYCODE_TAB:                  return ImGuiKey_Tab;
            case AKEYCODE_DPAD_LEFT:            return ImGuiKey_LeftArrow;
            case AKEYCODE_DPAD_RIGHT:           return ImGuiKey_RightArrow;
            case AKEYCODE_DPAD_UP:              return ImGuiKey_UpArrow;
            case AKEYCODE_DPAD_DOWN:            return ImGuiKey_DownArrow;
            case AKEYCODE_PAGE_UP:              return ImGuiKey_PageUp;
            case AKEYCODE_PAGE_DOWN:            return ImGuiKey_PageDown;
            case AKEYCODE_MOVE_HOME:            return ImGuiKey_Home;
            case AKEYCODE_MOVE_END:             return ImGuiKey_End;
            case AKEYCODE_INSERT:               return ImGuiKey_Insert;
            case AKEYCODE_FORWARD_DEL:          return ImGuiKey_Delete;
            case AKEYCODE_DEL:                  return ImGuiKey_Backspace;
            case AKEYCODE_SPACE:                return ImGuiKey_Space;
            case AKEYCODE_ENTER:                return ImGuiKey_Enter;
            case AKEYCODE_ESCAPE:               return ImGuiKey_Escape;
            case AKEYCODE_APOSTROPHE:           return ImGuiKey_Apostrophe;
            case AKEYCODE_COMMA:                return ImGuiKey_Comma;
            case AKEYCODE_MINUS:                return ImGuiKey_Minus;
            case AKEYCODE_PERIOD:               return ImGuiKey_Period;
            case AKEYCODE_SLASH:                return ImGuiKey_Slash;
            case AKEYCODE_SEMICOLON:            return ImGuiKey_Semicolon;
            case AKEYCODE_EQUALS:               return ImGuiKey_Equal;
            case AKEYCODE_LEFT_BRACKET:         return ImGuiKey_LeftBracket;
            case AKEYCODE_BACKSLASH:            return ImGuiKey_Backslash;
            case AKEYCODE_RIGHT_BRACKET:        return ImGuiKey_RightBracket;
            case AKEYCODE_GRAVE:                return ImGuiKey_GraveAccent;
            case AKEYCODE_CAPS_LOCK:            return ImGuiKey_CapsLock;
            case AKEYCODE_SCROLL_LOCK:          return ImGuiKey_ScrollLock;
            case AKEYCODE_NUM_LOCK:             return ImGuiKey_NumLock;
            case AKEYCODE_SYSRQ:                return ImGuiKey_PrintScreen;
            case AKEYCODE_BREAK:                return ImGuiKey_Pause;
            case AKEYCODE_NUMPAD_0:             return ImGuiKey_Keypad0;
            case AKEYCODE_NUMPAD_1:             return ImGuiKey_Keypad1;
            case AKEYCODE_NUMPAD_2:             return ImGuiKey_Keypad2;
            case AKEYCODE_NUMPAD_3:             return ImGuiKey_Keypad3;
            case AKEYCODE_NUMPAD_4:             return ImGuiKey_Keypad4;
            case AKEYCODE_NUMPAD_5:             return ImGuiKey_Keypad5;
            case AKEYCODE_NUMPAD_6:             return ImGuiKey_Keypad6;
            case AKEYCODE_NUMPAD_7:             return ImGuiKey_Keypad7;
            case AKEYCODE_NUMPAD_8:             return ImGuiKey_Keypad8;
            case AKEYCODE_NUMPAD_9:             return ImGuiKey_Keypad9;
            case AKEYCODE_NUMPAD_DOT:           return ImGuiKey_KeypadDecimal;
            case AKEYCODE_NUMPAD_DIVIDE:        return ImGuiKey_KeypadDivide;
            case AKEYCODE_NUMPAD_MULTIPLY:      return ImGuiKey_KeypadMultiply;
            case AKEYCODE_NUMPAD_SUBTRACT:      return ImGuiKey_KeypadSubtract;
            case AKEYCODE_NUMPAD_ADD:           return ImGuiKey_KeypadAdd;
            case AKEYCODE_NUMPAD_ENTER:         return ImGuiKey_KeypadEnter;
            case AKEYCODE_NUMPAD_EQUALS:        return ImGuiKey_KeypadEqual;
            case AKEYCODE_CTRL_LEFT:            return ImGuiKey_LeftCtrl;
            case AKEYCODE_SHIFT_LEFT:           return ImGuiKey_LeftShift;
            case AKEYCODE_ALT_LEFT:             return ImGuiKey_LeftAlt;
            case AKEYCODE_META_LEFT:            return ImGuiKey_LeftSuper;
            case AKEYCODE_CTRL_RIGHT:           return ImGuiKey_RightCtrl;
            case AKEYCODE_SHIFT_RIGHT:          return ImGuiKey_RightShift;
            case AKEYCODE_ALT_RIGHT:            return ImGuiKey_RightAlt;
            case AKEYCODE_META_RIGHT:           return ImGuiKey_RightSuper;
            case AKEYCODE_MENU:                 return ImGuiKey_Menu;
            case AKEYCODE_0:                    return ImGuiKey_0;
            case AKEYCODE_1:                    return ImGuiKey_1;
            case AKEYCODE_2:                    return ImGuiKey_2;
            case AKEYCODE_3:                    return ImGuiKey_3;
            case AKEYCODE_4:                    return ImGuiKey_4;
            case AKEYCODE_5:                    return ImGuiKey_5;
            case AKEYCODE_6:                    return ImGuiKey_6;
            case AKEYCODE_7:                    return ImGuiKey_7;
            case AKEYCODE_8:                    return ImGuiKey_8;
            case AKEYCODE_9:                    return ImGuiKey_9;
            case AKEYCODE_A:                    return ImGuiKey_A;
            case AKEYCODE_B:                    return ImGuiKey_B;
            case AKEYCODE_C:                    return ImGuiKey_C;
            case AKEYCODE_D:                    return ImGuiKey_D;
            case AKEYCODE_E:                    return ImGuiKey_E;
            case AKEYCODE_F:                    return ImGuiKey_F;
            case AKEYCODE_G:                    return ImGuiKey_G;
            case AKEYCODE_H:                    return ImGuiKey_H;
            case AKEYCODE_I:                    return ImGuiKey_I;
            case AKEYCODE_J:                    return ImGuiKey_J;
            case AKEYCODE_K:                    return ImGuiKey_K;
            case AKEYCODE_L:                    return ImGuiKey_L;
            case AKEYCODE_M:                    return ImGuiKey_M;
            case AKEYCODE_N:                    return ImGuiKey_N;
            case AKEYCODE_O:                    return ImGuiKey_O;
            case AKEYCODE_P:                    return ImGuiKey_P;
            case AKEYCODE_Q:                    return ImGuiKey_Q;
            case AKEYCODE_R:                    return ImGuiKey_R;
            case AKEYCODE_S:                    return ImGuiKey_S;
            case AKEYCODE_T:                    return ImGuiKey_T;
            case AKEYCODE_U:                    return ImGuiKey_U;
            case AKEYCODE_V:                    return ImGuiKey_V;
            case AKEYCODE_W:                    return ImGuiKey_W;
            case AKEYCODE_X:                    return ImGuiKey_X;
            case AKEYCODE_Y:                    return ImGuiKey_Y;
            case AKEYCODE_Z:                    return ImGuiKey_Z;
            case AKEYCODE_F1:                   return ImGuiKey_F1;
            case AKEYCODE_F2:                   return ImGuiKey_F2;
            case AKEYCODE_F3:                   return ImGuiKey_F3;
            case AKEYCODE_F4:                   return ImGuiKey_F4;
            case AKEYCODE_F5:                   return ImGuiKey_F5;
            case AKEYCODE_F6:                   return ImGuiKey_F6;
            case AKEYCODE_F7:                   return ImGuiKey_F7;
            case AKEYCODE_F8:                   return ImGuiKey_F8;
            case AKEYCODE_F9:                   return ImGuiKey_F9;
            case AKEYCODE_F10:                  return ImGuiKey_F10;
            case AKEYCODE_F11:                  return ImGuiKey_F11;
            case AKEYCODE_F12:                  return ImGuiKey_F12;
            default:                            return ImGuiKey_None;
        }
    }
}
