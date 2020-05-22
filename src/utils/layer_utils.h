#ifndef _HK_IMPL_LAYER_UTILS_H_
#define _HK_IMPL_LAYER_UTILS_H_

#include <stdarg.h>

typedef enum _HK_PRINT_LEVEL {
	_HK_PL_INFO	= 0,
	_HK_PL_WARNING = 1,
	_HK_PL_ERROR   = 2,
	_HK_PL_FATAL   = 3
} _HK_PRINT_LEVEL;

#ifdef __cplusplus
extern "C" {
#endif
void _hk_printf(_HK_PRINT_LEVEL level, const char* layer, const char* format, ...);
void _hk_p_info(const char* layer, const char* format, ...);
void _hk_p_warning(const char* layer, const char* format, ...);
void _hk_p_error(const char* layer, const char* format, ...);
void _hk_p_fatal(const char* layer, const char* format, ...);

void _hk_vprintf(_HK_PRINT_LEVEL level, const char* layer, const char* format, va_list args);
void _hk_vp_info(const char* layer, const char* format, va_list args);
void _hk_vp_warning(const char* layer, const char* format, va_list args);
void _hk_vp_error(const char* layer, const char* format, va_list args);
void _hk_vp_fatal(const char* layer, const char* format, va_list args);

#ifdef __cplusplus
}
#endif
#endif /*_HK_IMPL_LAYER_UTILS_H_*/