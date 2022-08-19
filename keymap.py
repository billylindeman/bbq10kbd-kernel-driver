#!/usr/bin/env python3
keymap = [0]*255

#row 1
keymap[0x51] = 'KEY_Q'
keymap[0x57] = 'KEY_W'
keymap[0x45] = 'KEY_E'
keymap[0x52] = 'KEY_R'
keymap[0x54] = 'KEY_T'
keymap[0x59] = 'KEY_Y'
keymap[0x55] = 'KEY_U'
keymap[0x49] = 'KEY_I'
keymap[0x4F] = 'KEY_O'
keymap[0x50] = 'KEY_P'

#row 2
keymap[0x41] = 'KEY_A'
keymap[0x53] = 'KEY_S'
keymap[0x44] = 'KEY_D'
keymap[0x46] = 'KEY_F'
keymap[0x47] = 'KEY_G'
keymap[0x48] = 'KEY_H'
keymap[0x4A] = 'KEY_J'
keymap[0x4B] = 'KEY_K'
keymap[0x4C] = 'KEY_L'
keymap[0x08] = 'KEY_BACKSPACE'

#row 3
keymap[0x1A] = 'KEY_LEFTALT'
keymap[0x5A] = 'KEY_Z'
keymap[0x58] = 'KEY_X'
keymap[0x43] = 'KEY_C'
keymap[0x56] = 'KEY_V'
keymap[0x42] = 'KEY_B'
keymap[0x4E] = 'KEY_N'
keymap[0x4D] = 'KEY_M'
keymap[0x24] = 'KEY_COMMA'
keymap[0x0A] = 'KEY_ENTER'


#row 4
keymap[0x1B] = 'KEY_LEFTSHIFT'
keymap[0x7E] = 'KEY_LEFTCTRL'
keymap[0x20] = 'KEY_SPACE'
keymap[0x1D] = 'KEY_COPY'
keymap[0x1C] = 'KEY_RIGHTSHIFT'


#top row
keymap[0x06] = 'KEY_ESC'
keymap[0x11] = 0
keymap[0x05] = 0
keymap[0x07] = 0


print("#include <linux/input.h>")
print("")
print("static unsigned short bbq10kbd_keycodes[] = {")
for i in range(len(keymap)):
    end = '' if i%15 or i == 0  else '\n'
    print("{}, ".format(keymap[i]), end=end)
print("");
print("};")
