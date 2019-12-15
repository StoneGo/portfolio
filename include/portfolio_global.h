//
// Created by stone on 11/19/19.
//

#ifndef CTPAPI_PORTFOLIO_GLOBAL_H
#define CTPAPI_PORTFOLIO_GLOBAL_H



#ifndef SHARED_LIBRARY_EXPORT
#  ifdef _WINDOWS
#    define SHARED_LIBRARY_EXPORT     __declspec(dllexport)
#    define SHARED_LIBRARY_IMPORT     __declspec(dllimport)
#  elif (_UNIX)
#    define SHARED_LIBRARY_EXPORT
#    define SHARED_LIBRARY_IMPORT
#    define SHARED_LIBRARY_HIDDEN
#  endif
#endif


//SHARED_LIBRARY_EXPORT
#if defined(PORTFOLIO_LIBRARY)
#  define PORTFOLIO_API SHARED_LIBRARY_EXPORT
#else
#  define PORTFOLIO_API SHARED_LIBRARY_IMPORT
#endif

#define UNUSED(x) (void)(x)


#ifndef SafeDeletePtr
#define SafeDeletePtr(x) if ((x) != nullptr) {delete (x);(x) = nullptr;}
#endif //SafeDeletePtr

#ifndef SafeDeleteAry
#define SafeDeleteAry(x) if ((x) != nullptr) {delete[] (x);(x) = nullptr;}
#endif //SafeDeletePtr

#endif //CTPAPI_PORTFOLIO_GLOBAL_H
