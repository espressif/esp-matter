/**************************************************************************//**
 * Copyright 2022, Silicon Laboratories Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef SL_WFX_VERSION_H
#define SL_WFX_VERSION_H

/** X.x.x: Major version of the driver */
#define FMAC_DRIVER_VERSION_MAJOR      3
/** x.X.x: Minor version of the driver */
#define FMAC_DRIVER_VERSION_MINOR      5
/** x.x.X: Revision of the driver */
#define FMAC_DRIVER_VERSION_REVISION   0
/** For release candidates, this is set to 1..254
 * For official releases, this is set to 255 (FMAC_DRIVER_RC_RELEASE)
 * For development versions, this is set to 0 (FMAC_DRIVER_RC_DEVELOPMENT) */
#define FMAC_DRIVER_VERSION_RC         255

/** FMAC_DRIVER_VERSION_RC is set to FMAC_DRIVER_RC_RELEASE for official releases */
#define FMAC_DRIVER_RC_RELEASE         255
/** FMAC_DRIVER_VERSION_RC is set to FMAC_DRIVER_RC_DEVELOPMENT for development versions */
#define FMAC_DRIVER_RC_DEVELOPMENT     0

#define FMAC_DRIVER_VERSION_IS_RELEASE     (FMAC_DRIVER_VERSION_RC == FMAC_DRIVER_RC_RELEASE)
#define FMAC_DRIVER_VERSION_IS_DEVELOPMENT (FMAC_DRIVER_VERSION_RC == FMAC_DRIVER_RC_DEVELOPMENT)
#define FMAC_DRIVER_VERSION_IS_RC          ((FMAC_DRIVER_VERSION_RC != FMAC_DRIVER_RC_RELEASE) \
                                            && (FMAC_DRIVER_VERSION_RC != FMAC_DRIVER_RC_DEVELOPMENT))

/* Some helper defines to get a version string */
#define FMAC_DRIVER_VERSTR2(x) #x
#define FMAC_DRIVER_VERSTR(x) FMAC_DRIVER_VERSTR2(x)
#if FMAC_DRIVER_VERSION_IS_RELEASE
#define FMAC_DRIVER_VERSION_STRING_SUFFIX ""
#elif FMAC_DRIVER_VERSION_IS_DEVELOPMENT
#define FMAC_DRIVER_VERSION_STRING_SUFFIX "d"
#else
#define FMAC_DRIVER_VERSION_STRING_SUFFIX "rc" FMAC_DRIVER_VERSTR(FMAC_DRIVER_VERSION_RC)
#endif

/** Provides the version of the driver */
#define FMAC_DRIVER_VERSION   ((FMAC_DRIVER_VERSION_MAJOR) << 24   | (FMAC_DRIVER_VERSION_MINOR) << 16 \
                               | (FMAC_DRIVER_VERSION_REVISION) << 8 | (FMAC_DRIVER_VERSION_RC))
/** Provides the version of the driver as string */
#define FMAC_DRIVER_VERSION_STRING     FMAC_DRIVER_VERSTR(FMAC_DRIVER_VERSION_MAJOR) "." \
  FMAC_DRIVER_VERSTR(FMAC_DRIVER_VERSION_MINOR) "."                                      \
  FMAC_DRIVER_VERSTR(FMAC_DRIVER_VERSION_REVISION)                                       \
  FMAC_DRIVER_VERSION_STRING_SUFFIX

#endif // SL_WFX_VERSION_H
