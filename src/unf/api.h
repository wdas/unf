#ifndef UNF_API_H
#define UNF_API_H

#include "pxr/base/arch/export.h"

#if defined(UNF_STATIC)
#   define UNF_API
#   define UNF_API_TEMPLATE_CLASS(...)
#   define UNF_API_TEMPLATE_STRUCT(...)
#   define UNF_LOCAL
#else
#   if defined(UNF_EXPORTS)
#       define UNF_API ARCH_EXPORT
#       define UNF_API_TEMPLATE_CLASS(...) ARCH_EXPORT_TEMPLATE(class, __VA_ARGS__)
#       define UNF_API_TEMPLATE_STRUCT(...) ARCH_EXPORT_TEMPLATE(struct, __VA_ARGS__)
#   else
#       define UNF_API ARCH_IMPORT
#       define UNF_API_TEMPLATE_CLASS(...) ARCH_IMPORT_TEMPLATE(class, __VA_ARGS__)
#       define UNF_API_TEMPLATE_STRUCT(...) ARCH_IMPORT_TEMPLATE(struct, __VA_ARGS__)
#   endif
#   define UNF_LOCAL ARCH_HIDDEN
#endif

#endif