#ifndef LINK_CURVE25519_DEF
#define LINK_CURVE25519_DEF
#if defined (WIN32)
  #ifndef __cplusplus
	  #define snprintf    _snprintf
  #endif
#endif

#if defined (WIN32) && defined (CURVE25519_EXPORTS)
	#define LINK_CURVE25519 __declspec(dllexport)
#elif defined (WIN32) && defined (CURVE25519_IMPORTS)
	#define LINK_CURVE25519 __declspec(dllimport)
#else
	#define LINK_CURVE25519
#endif
#endif
