#!/usr/bin/python
import os
import argparse
import json


def write_header(out):
    out.write(
        """#ifndef _HIKARI_ICD_DEFINITION_H_
#define _HIKARI_ICD_DEFINITION_H_ 1

#include "hikari.h"
""")


def write_footer(out):
    out.write(
        """
#endif /*_HIKARI_ICD_DEFINITION_H_*/""")


def get_definition(name, definition):
    is_pointer = ('pointer' in definition) and definition['pointer']
    is_constant = ('constant' in definition) and definition['constant']
    is_array = ('array' in definition) and definition['array']
    elem_count = definition['count'] if ('count' in definition) else 1

    res = "%s%s%s %s" % (
        "const " if is_constant else "",
        definition['type'],
        "*" if is_pointer else "",
        name)

    if is_array:
        return "%s[%s]" % (res, elem_count)
    else:
        return res


def write_functions(data, out, suffix):
    out.write("\n")
    for entry, value in data['functions'].items():
        ret_val = value['return'] if 'return' in value else 'void'
        out.write("%s %s%s(\n" % (ret_val, entry, suffix))
        out.write(",\n".join(["\t%s" % (get_definition(entry2, value2))
                              for entry2, value2 in value['params'].items()]))
        out.write(");\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('in')
    parser.add_argument('out')
    parser.add_argument('--suffix', nargs=1)
    opts = vars(parser.parse_args())

    with open(opts['in'], 'r', encoding='utf-8') as infile:
        data = json.load(infile)

    with open(opts['out'], 'w') as outfile:
        write_header(outfile)
        write_functions(data, outfile, opts['suffix'][0] if opts['suffix'] else '')
        write_footer(outfile)
