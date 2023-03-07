#ifndef LINK_CHACHA20POLY1305_DEF
#define LINK_CHACHA20POLY1305_DEF
#if defined (WIN32)
  #ifndef __cplusplus
	  #define snprintf    _snprintf
  #endif
#endif

#if defined (WIN32) && defined (CHACHA20POLY1305_EXPORTS)
	#define LINK_CHACHA20POLY1305 __declspec(dllexport)
#elif defined (WIN32) && defined (CHACHA20POLY1305_IMPORTS)
	#define LINK_CHACHA20POLY1305 __declspec(dllimport)
#else
	#define LINK_CHACHA20POLY1305
#endif
#endif
