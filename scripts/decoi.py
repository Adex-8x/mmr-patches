import json
import random
import hashlib
from arc4 import ARC4

WIDE_MAP = {i: i + 0xFEE0 for i in range(0x21, 0x7F)}
WIDE_MAP[0x20] = 0x3000
BANNED_CHARS = '\'-"'
DEFAULT_DATA = '''#include <pmdsky.h>
#include <cot.h>
#include "extern.h"

#define SECTION_CREATE __attribute__((used))__attribute__((section(".CREATE")))
#define SECTION_UPDATE __attribute__((used))__attribute__((section(".UPDATE")))
#define SECTION_DATA __attribute__((used))__attribute__((section(".PAIN")))

struct top_dialogue {
	uint8_t* string;
	uint32_t size;
	uint8_t key[0x10];
	bool is_ketsuban;
};

#ifdef DECOI_ENABLED
'''

top_dialogue = "SECTION_DATA struct top_dialogue TOP_DIALOGUE[] = {\n"

with open("decoi_top.json", "r") as f:
	dialogue = json.load(f)
	
def hexify(stuff: bytes) -> str:
	key_rep = ""
	for s in stuff:
		key_rep += hex(s) + ","
	return key_rep[:-1]
	
def ascii_to_wide(constant: str):
	new_constant = ""
	in_text_tag = False
	for c in constant:
		if c == '[':
			in_text_tag = True
			new_constant += c
		elif c == ']':
			in_text_tag = False
			new_constant += c
		else:
			new_constant += c.translate(WIDE_MAP) if not in_text_tag and c.isascii() and c not in BANNED_CHARS else c
	return new_constant

# a magic number a day keeps the sanity away
random.seed(581)
	
data = DEFAULT_DATA
castle_counter = 0
ketsuban_counter = 0
encountered_start = False
for is_ketsuban,line in dialogue:
	if not encountered_start:
		if line == "なんでもできる！":
			encountered_start = True
		continue
	speaker = ""
	counter = None
	if is_ketsuban:
		ketsuban_counter += 1
		speaker = "KETSUBAN"
		counter = ketsuban_counter
		line = "[FT:1]" + ascii_to_wide(line)
	else:
		castle_counter += 1
		speaker = "CASTLE"
		counter = castle_counter
		line = "[FT:0]" + line
	line = line.encode('shift-jis') + b'\x00'
	marker = f"{speaker}_DIALOGUE_{counter:02d}"
	data += f"SECTION_DATA uint8_t {marker}[] = {{"
	# Add the encrypted string
	key = random.randbytes(0x10)
	key_rep = f"{{{hexify(key)}}}"
	cipher = ARC4(key)
	encrypted_string = hexify(cipher.encrypt(line))
	data += encrypted_string
	top_dialogue += f"\t{{{marker},{len(line)},{key_rep},{str(is_ketsuban).lower()}}},\n"
	data += "};\n"
	
top_dialogue += "};"

data += "\n" + top_dialogue
data += "\n#endif\n"

with open("src/decoi_top.h", "w") as f:
	f.write(data)
	
	
# Dungeon segment yippee

DEFAULT_DATA = '''#include <pmdsky.h>
#include <cot.h>
#include "extern.h"

#define SECTION_DUNGEON __attribute__((used))__attribute__((section(".text.dungeon")))
#define SECTION_CHAINS __attribute__((used))__attribute__((section(".data.chains")))
#define SECTION_HELPER __attribute__((used))__attribute__((section(".text.dungeon_helper")))

#ifdef DECOI_ENABLED
'''
	
with open("decoi_chains.json", "r") as f:
	dialogue = json.load(f)
	
m = hashlib.md5()
data = DEFAULT_DATA
dungeon_dialogue = "SECTION_CHAINS uint8_t* CHAINS_DIALOGUE[] = {"
cipher = ARC4((581).to_bytes(2, 'little'))
for i,line in enumerate(dialogue):
	thingy = ascii_to_wide(line)
	true_thingy = thingy.encode('shift-jis')
	m.update(true_thingy)
	# thingy = thingy.replace("\n", r"\n").replace('"', r'\"')
	encrypted_thingy = cipher.encrypt(true_thingy + b"\x00")
	marker = f"CHAINS_DIALOGUE_{i:02d}"
	data += f"SECTION_CHAINS uint8_t {marker}[] = {{{hexify(encrypted_thingy)}}};\n"
	dungeon_dialogue += marker + ","
	
dialogue_hash = m.digest()
dungeon_dialogue = dungeon_dialogue[:-1] + "};\n"
data += "\n" + dungeon_dialogue

# The final stage
PLAINTEXT_FINAL_STAGE = "/answer file: decoi_dungeon.c line: 337 response: [???]".encode('ascii') + b'\x00'
cipher = ARC4(dialogue_hash)
result = cipher.encrypt(PLAINTEXT_FINAL_STAGE)
CIPHERTEXT_FINAL_STAGE = hexify(result)
data += "\n\n" + f"SECTION_CHAINS uint8_t FINAL_STAGE[] = {{{CIPHERTEXT_FINAL_STAGE}}};\n"
data += "\n#endif\n"

with open("src/decoi_dungeon.h", "w") as f:
	f.write(data)