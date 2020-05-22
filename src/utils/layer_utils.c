#include "layer_utils.h"

#include <stdio.h>

void _hk_printf(_HK_PRINT_LEVEL level, const char* layer, const char* format, ...)
{
	va_list va;

	va_start(va, format);
	_hk_vprintf(level, layer, format, va);
	va_end(va);
}

void _hk_p_info(const char* layer, const char* format, ...)
{
	va_list va;
	va_start(va, format);
	_hk_vp_info(layer, format, va);
	va_end(va);
}

void _hk_p_warning(const char* layer, const char* format, ...)
{
	va_list va;
	va_start(va, format);
	_hk_vp_warning(layer, format, va);
	va_end(va);
}

void _hk_p_error(const char* layer, const char* format, ...)
{
	va_list va;
	va_start(va, format);
	_hk_vp_error(layer, format, va);
	va_end(va);
}

void _hk_p_fatal(const char* layer, const char* format, ...)
{
	va_list va;
	va_start(va, format);
	_hk_vp_fatal(layer, format, va);
	va_end(va);
}

void _hk_vp_info(const char* layer, const char* format, va_list args)
{
	_hk_vprintf(_HK_PL_INFO, layer, format, args);
}

void _hk_vp_warning(const char* layer, const char* format, va_list args)
{
	_hk_vprintf(_HK_PL_WARNING, layer, format, args);
}

void _hk_vp_error(const char* layer, const char* format, va_list args)
{
	_hk_vprintf(_HK_PL_ERROR, layer, format, args);
}

void _hk_vp_fatal(const char* layer, const char* format, va_list args)
{
	_hk_vprintf(_HK_PL_FATAL, layer, format, args);
}

void _hk_vprintf(_HK_PRINT_LEVEL level, const char* layer, const char* format, va_list args)
{
	FILE* stream;

	switch (level) {
	case _HK_PL_INFO:
		stream = stdout;
		fprintf(stream, "INFO    [%s]: ", layer);
		break;
	case _HK_PL_WARNING:
		stream = stdout;
		fprintf(stream, "WARNING [%s]: ", layer);
		break;
	case _HK_PL_ERROR:
		stream = stderr;
		fprintf(stream, "ERROR   [%s]: ", layer);
		break;
	default:
	case _HK_PL_FATAL:
		stream = stderr;
		fprintf(stream, "FATAL   [%s]: ", layer);
		break;
	}

	vfprintf(stream, format, args);
}