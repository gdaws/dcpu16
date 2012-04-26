#!/bin/env py

token_entry_symbols = [
    (9, 9,     "WHITESPACE"),
    (10, 10,   "NEWLINE"),
    (13, 13,   "NEWLINE"),
    (32, 32,   "WHITESPACE"),
    (45, 45,   "NUMBER"),
    (48, 57,   "NUMBER"),
    (59, 59,   "COMMENT"),
    (65, 90,   "WORD"),
    (97, 122,  "WORD"),
    (95, 95,   "WORD"),
    (91, 91,   "OPEN_SUBSCRIPT"),
    (93, 93,   "CLOSE_SUBSCRIPT"),
    (44, 44,   "COMMA"),
    (43, 43,   "PLUS"),
    (58, 58,   "LABEL"),
    (34, 34,   "STRING")
]

word_symbols = [
    (65, 90),
    (97, 122),
    (95, 95),
    (48, 57)
]

number_symbols = [
    (45, 45),
    (48, 57),
    (65, 70),
    (97, 102),
    (120, 120)
]

string_symbols = [
    (32, 33),
    (35, 126)
]


def token_identifier(i, symbol_ranges):
    for token_range in symbol_ranges:
        if i >= token_range[0] and i <= token_range[1]:
            return token_range[2]
    return "ERROR"


def token_set(i, symbol_ranges):
    for token_range in symbol_ranges:
        if i >= token_range[0] and i <= token_range[1]:
            return "1"
    return "0"

output = ""
for i in range(127):
    output += token_set(i, string_symbols) + ","
    if (i + 1) % 8 == 0:
        output += "\n"
print output
