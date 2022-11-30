
#ifndef POPPLER_QT5_EXPORT_H
#define POPPLER_QT5_EXPORT_H

#ifdef POPPLER_QT5_STATIC_DEFINE
#  define POPPLER_QT5_EXPORT
#  define POPPLER_QT5_NO_EXPORT
#else
#  ifndef POPPLER_QT5_EXPORT
#    ifdef poppler_qt5_EXPORTS
        /* We are building this library */
#      define POPPLER_QT5_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define POPPLER_QT5_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef POPPLER_QT5_NO_EXPORT
#    define POPPLER_QT5_NO_EXPORT 
#  endif
#endif

#ifndef POPPLER_QT5_DEPRECATED
#  define POPPLER_QT5_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef POPPLER_QT5_DEPRECATED_EXPORT
#  define POPPLER_QT5_DEPRECATED_EXPORT POPPLER_QT5_EXPORT POPPLER_QT5_DEPRECATED
#endif

#ifndef POPPLER_QT5_DEPRECATED_NO_EXPORT
#  define POPPLER_QT5_DEPRECATED_NO_EXPORT POPPLER_QT5_NO_EXPORT POPPLER_QT5_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef POPPLER_QT5_NO_DEPRECATED
#    define POPPLER_QT5_NO_DEPRECATED
#  endif
#endif

#endif /* POPPLER_QT5_EXPORT_H */
