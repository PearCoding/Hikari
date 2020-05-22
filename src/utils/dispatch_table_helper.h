#pragma once

#include "hikari.h"

typedef struct _HkDispatchMapper
{
	const char* name;
	PFN_hkVoidFunction func;
} _HkDispatchMap;

#define _HK_DISPATCH_ENTRY(fn) {#fn, (PFN_hkVoidFunction)fn}
#define _HK_DISPATCH_ENTRY_S(fn,suf) {#fn, (PFN_hkVoidFunction)fn##suf}
#define _HK_DISPATCH_ENTRY_END() {NULL, NULL}