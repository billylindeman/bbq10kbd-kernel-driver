#!/usr/bin/env python3


#q=0x71
#w=0x77
#e=0x65
#r=0x72
#t=0x74
#y=0x79
#u=0x75
#i=0x69
#o=0x6f
#p=0x70
#a=0x61
#s=0x73
#d=0x64
#f=0x66
#g=0x67
#h=0x68
#j=0x6a
#k=0x6b
#l=0x6c
#z=0x7a
#x=0x78
#c=0x63
#v=0x76
#b=0x62
#n=0x6e
#m=0x6d
#
#
#$=0x24
#<backspace>=0x08
#<return>=0x0a

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


#alt
keymap[0x2F] = 'KEY_SLASH'
keymap[0x06] = 'KEY_ESC'
keymap[0x60] = 'KEY_TAB'


print("static unsigned short bbq10_keycodes[] = {\n")
for i in range(len(keymap)):
    end = '' if i%14 else '\n'
    print("{}, ".format(keymap[i]), end=end)
print("};")
