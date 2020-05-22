#!/usr/bin/python
import os
import argparse
import json
import re


def write_header(out):
    out.write(
        """#ifndef _HIKARI_H_
#define _HIKARI_H_ 1

#ifndef HK_NO_STD_INCLUDES
#include <stddef.h>
#include <stdint.h>
#endif/*HK_NO_STD_INCLUDES*/

#if defined(_MSC_VER)
#define HK_EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
#ifdef __cplusplus
#define HK_EXPORT extern "C" __attribute__((visibility("default")))
#else
#define HK_EXPORT __attribute__((visibility("default")))
#endif
#else
#define EXPORT
#pragma warning Unknown dynamic link import / export semantics.
#endif

#ifndef HKAPIENTRY
#define HKAPIENTRY
#endif
#ifndef HKAPI
#ifdef __cplusplus
#define HKAPI extern "C"
#else
#define HKAPI extern
#endif
#endif

#define HK_MAKE_VERSION(major, minor) \
	(((major) << 12) | (minor))

#define HK_API_VERSION_1_0 HK_MAKE_VERSION(1, 0)

#define HK_VERSION_MAJOR(version) ((uint32_t)(version) >> 12)
#define HK_VERSION_MINOR(version) ((uint32_t)(version)&0x3ff)

typedef void HkVoid;
typedef unsigned int HkEnum;
typedef float HkFloat;
typedef double HkDouble;
typedef unsigned char HkBool;

typedef void(HKAPIENTRY* PFN_hkVoidFunction)(void);
typedef void*(HKAPIENTRY* PFN_hkAllocationFunction)(void* pUserData, size_t size, size_t alignment);
typedef void*(HKAPIENTRY* PFN_hkReallocationFunction)(void* pUserData, void* pOriginal,
													  size_t size, size_t alignment);
typedef void(HKAPIENTRY* PFN_hkFreeFunction)(void* pUserData, void* pData, size_t size);
""")


def write_footer(out):
    out.write(
        """
/*Init function*/
#ifndef HK_NO_INIT_FUNCTION
HK_EXPORT PFN_hkVoidFunction hkGetInstanceProcAddr(HkInstance instance, const char* pName);
#endif
#endif /*_HIKARI_H_*/""")


def write_constants(data, out):
    out.write("\n")
    for entry, value in data['constants'].items():
        out.write("#define %s %s\n" % (entry, value))


def write_handles(data, out):
    out.write("\n")
    out.write("#define HK_DEFINE_HANDLE(obj) typedef struct obj##_OBJ* obj\n")
    for entry in data['handles']:
        out.write("HK_DEFINE_HANDLE(%s);\n" % entry)
    out.write("#undef HK_DEFINE_HANDLE\n")


def write_structure_type_enum(data, out):
    ext_structs = [e for e, v in data['structs'].items() if (
        'extensible' in v) and v['extensible']]

    ext_structs.append("HkLoaderInstanceCreateInfo")

    out.write("typedef enum HkStructureType {\n")
    c = 0
    for e in ext_structs:
        name = e[2:]
        entry = "_".join(map(str.upper, re.sub(
            r"([A-Z])", r" \1", name).split()))
        entry = "HK_STRUCTURE_TYPE_%s" % entry
        out.write("\t%s = %i,\n" % (entry,c))
        c = c + 1
    out.write("} HkStructureType;\n")


def write_enums(data, out):
    out.write("\n")
    for entry, value in data['enums'].items():
        out.write("typedef enum %s {\n" % entry)
        for entry2, value2 in value['entries'].items():
            out.write("\t%s = %s,\n" % (entry2, value2))
        if 'last' in value:
            out.write("\t%s\n" % value['last'])
        out.write("} %s;\n" % entry)
    write_structure_type_enum(data, out)


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


def write_struct_extensible(out):
    out.write("\tHkStructureType sType;\n\tvoid* pNext;\n")


def write_structs(data, out):
    out.write("\n")
    for entry, value in data['structs'].items():
        out.write("typedef struct %s {\n" % entry)

        if ('extensible' in value) and value['extensible']:
            write_struct_extensible(out)

        if 'entries' in value:
            for entry2, value2 in value['entries'].items():
                out.write("\t%s;\n" % (get_definition(entry2, value2)))

        out.write("} %s;\n" % entry)


def write_functions(data, out):
    out.write("\n")
    for entry, value in data['functions'].items():
        ret_val = value['return'] if 'return' in value else 'void'
        out.write("typedef %s (HKAPIENTRY* PFN_%s)(\n" % (ret_val, entry))
        out.write(",\n".join(["\t%s" % (get_definition(entry2, value2))
                              for entry2, value2 in value['params'].items()]))
        out.write(");\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('in')
    parser.add_argument('out')
    opts = vars(parser.parse_args())

    with open(opts['in'], 'r', encoding='utf-8') as infile:
        data = json.load(infile)

    with open(opts['out'], 'w') as outfile:
        write_header(outfile)
        write_constants(data, outfile)
        write_handles(data, outfile)
        write_enums(data, outfile)
        write_structs(data, outfile)
        write_functions(data, outfile)
        write_footer(outfile)
