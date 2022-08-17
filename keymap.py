#!/usr/bin/env python3
keymap = [0]*255

keymap[0x71] = 'KEY_Q'
keymap[0x77] = 'KEY_W'
keymap[0x65] = 'KEY_E'
keymap[0x72] = 'KEY_R'
keymap[0x74] = 'KEY_T'
keymap[0x79] = 'KEY_Y'
keymap[0x75] = 'KEY_U'
keymap[0x69] = 'KEY_I'
keymap[0x6f] = 'KEY_O'
keymap[0x70] = 'KEY_P'
keymap[0x61] = 'KEY_A'
keymap[0x73] = 'KEY_S'
keymap[0x64] = 'KEY_D'
keymap[0x66] = 'KEY_F'
keymap[0x67] = 'KEY_G'
keymap[0x68] = 'KEY_H'
keymap[0x6a] = 'KEY_J'
keymap[0x6b] = 'KEY_K'
keymap[0x6c] = 'KEY_L'
keymap[0x7a] = 'KEY_Z'
keymap[0x78] = 'KEY_X'
keymap[0x63] = 'KEY_C'
keymap[0x76] = 'KEY_V'
keymap[0x62] = 'KEY_B'
keymap[0x6e] = 'KEY_N'
keymap[0x6d] = 'KEY_M'

keymap[0x24] = 'KEY_DOLLAR'
keymap[0x08] = 'KEY_BACKSPACE'
keymap[0x0a] = 'KEY_ENTER'
keymap[0x20] = 'KEY_SPACE'

#top row
keymap[0x06] = 'KEY_ESC'

#alt row 1
keymap[0x23] = 'KEY_NUMERIC_POUND'
keymap[0x31] = 'KEY_1'
keymap[0x32] = 'KEY_2'
keymap[0x33] = 'KEY_3'
keymap[0x28] = 'KEY_KPLEFTPAREN'
keymap[0x29] = 'KEY_KPRIGHTPAREN'
#keymap[0x5F] = 'KEY_UNDERSCORE'

keymap[0x2D] = 'KEY_MINUS'
keymap[0x2B] = 'KEY_KPPLUS'
keymap[0x2A] = 'KEY_KPASTERISK'
keymap[0x33] = 'KEY_4'
keymap[0x34] = 'KEY_5'
keymap[0x35] = 'KEY_6'
keymap[0x2F] = 'KEY_SLASH'
keymap[0x3A] = 'KEY_SEMICOLON'

keymap[0x37] = 'KEY_7'
keymap[0x38] = 'KEY_8'
keymap[0x39] = 'KEY_9'
keymap[0x3F] = 'KEY_QUESTION'

#speaker
keymap[0x60] = 'KEY_TAB'

print("#include <linux/input.h>")
print("")
print("static unsigned short bbq10kbd_keycodes[] = {")
for i in range(len(keymap)):
    end = '' if i%15 or i == 0  else '\n'
    print("{}, ".format(keymap[i]), end=end)
print("");
print("};")
