/*
 *  Copyright (c) 2018, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <stdbool.h>

#include "openthread-posix-config.h"

#include "openthread/config.h"

#include <assert.h>
#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#ifdef __linux__
#include <sys/prctl.h>
#endif

#ifndef HAVE_LIBEDIT
#define HAVE_LIBEDIT 0
#endif

#ifndef HAVE_LIBREADLINE
#define HAVE_LIBREADLINE 0
#endif

#include "openthread/platform/radio.h"

#include "common/code_utils.hpp"
#include "common/logging.hpp"
#include "lib/platform/exit_code.h"
#include "openthread/openthread-system.h"

#ifndef OPENTHREAD_ENABLE_COVERAGE
#define OPENTHREAD_ENABLE_COVERAGE 0
#endif

typedef struct PosixConfig{
  otPlatformConfig mPlatformConfig;      ///< Platform configuration.
  otLogLevel       mLogLevel;            ///< Debug level of logging.
  bool             mIsVerbose;           ///< Whether to print log to stderr.
} PosixConfig;

extern jmp_buf gResetJump;

#define SERIAL_PORT_NAME_MAX_LEN 40
extern char serialPort[SERIAL_PORT_NAME_MAX_LEN];
#define RADIO_URL_MAX_LEN 150
char radioUrl[RADIO_URL_MAX_LEN];

/**
 * This enumeration defines the argument return values.
 *
 */
enum {
  OT_POSIX_OPT_RADIO_URL      = 'r',
  OT_POSIX_OPT_DEBUG_LEVEL    = 'd',
  OT_POSIX_OPT_EZSP_INTERFACE = 'p',
  OT_POSIX_OPT_HELP           = 'h',
  OT_POSIX_OPT_VERBOSE        = 'v',
};

#define GETOPT_OPTION_STRING "r:d:p:hv"
static const struct option kOptions[] = {
  { "radio-url", required_argument, NULL, OT_POSIX_OPT_RADIO_URL },
  { "debug-level", required_argument, NULL, OT_POSIX_OPT_DEBUG_LEVEL },
  { "ezsp-interface", required_argument, NULL, OT_POSIX_OPT_EZSP_INTERFACE },
  { "help", no_argument, NULL, OT_POSIX_OPT_HELP },
  { "verbose", no_argument, NULL, OT_POSIX_OPT_VERBOSE },
  { 0, 0, 0, 0 }
};

static void PrintUsage(const char *aProgramName, FILE *aStream, int aExitCode)
{
  fprintf(aStream,
          "Syntax:\n"
          "    %s [Options]\n"
          "Options:\n"
          "    -r  --radio-url <RadioURL>    URL to the radio device. Must include iid=n with n=1,2, or 3.\n"
          "    -d  --debug-level <level>     Debug level for Spinel syslog logging.\n"
          "    -p  --ezsp-interface <name>   EZSP interface name.\n"
          "    -h  --help                    Display this usage information.\n"
          "    -v  --verbose                 Also log Spinel to stderr.\n",
          aProgramName);
  exit(aExitCode);
}

// returns number of arguments read. Negative numbers are errors.
static ssize_t ParseEtcConf(char *confFilePath, int aArgVectorSize, char *aArgVector[])
{
  ssize_t argCount = 0;
  FILE *fp = fopen(confFilePath, "r");
  if (fp == NULL) {
    fprintf(stderr, "Cannot open file %s\n", confFilePath);
    return 0; // no config file
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t read = 0;
  char *finger = NULL;
  while ((read = getline(&line, &len, fp)) != -1) {
    if ((finger = strstr(line, "#")) != NULL) {
      *finger = '\0'; // replace comment with NULL terminator
    }

    if (strlen(line) == 0) { // ignore empty (or comment) lines
      continue;
    }

    if ((finger = strstr(line, "=")) == NULL) { // ignore non-set lines
      continue;
    }

    *finger = '\0'; // replace with NULL terminator
    char *tmp = finger - 1;
    while ((*tmp == ' ') || (*tmp == '\t')) { // remove trailing whitespace
      *(tmp--) = '\0';
    }

    if (argCount == aArgVectorSize) {
      return ENOBUFS;
    }
    aArgVector[argCount] = malloc(sizeof(char) * (strlen(line) + 2 + 1)); // make room for "--"
    if (aArgVector[argCount] == NULL) {
      return ENOMEM;
    }
    sprintf(aArgVector[argCount], line[1] == '\0' ? "-%s" : "--%s", line);
    argCount++;
    //fprintf(stderr,"Found option: %s.\n",line);

    finger++; // step over '='
    while ((*finger == ' ') || (*finger == '\t')) { // remove leading whitespace
      finger++;
    }

    tmp = finger + strlen(finger) - 1; // set to the end
    while ((*tmp == ' ') || (*tmp == '\t') || (*tmp == '\n')) { // remove trailing whitespace
      *(tmp--) = '\0';
    }

    if ((*finger == *tmp) && ((*finger == '\"') || (*finger == '\''))) { // remove '"' or '\'' from beginning and end
      finger++;
      *(tmp--) = '\0';
    }

    if (*finger == '\0') { // no arguments for option
      continue;
    }

    if (argCount == aArgVectorSize) {
      return ENOBUFS;
    }
    aArgVector[argCount] = malloc(sizeof(char) * (strlen(finger) + 1));
    if (aArgVector[argCount] == NULL) {
      return ENOMEM;
    }
    strcpy(aArgVector[argCount], finger);
    argCount++;
    //fprintf(stderr,"Found argument: %s.\n",finger);
  }

  fclose(fp);
  if (line) {
    free(line);
  }

  return argCount;
}

static void ParseArg(int aArgCount, char *aArgVector[], PosixConfig *aConfig)
{
  optind = 1;

  while (true) {
    int index  = 0;
    int option = getopt_long(aArgCount, aArgVector,
                             GETOPT_OPTION_STRING,
                             kOptions, &index);

    if (option == -1) {
      break;
    }

    switch (option) {
      case OT_POSIX_OPT_DEBUG_LEVEL:
        aConfig->mLogLevel = (otLogLevel)atoi(optarg);
        break;
      case OT_POSIX_OPT_HELP:
        PrintUsage(aArgVector[0], stdout, OT_EXIT_SUCCESS);
        break;
      case OT_POSIX_OPT_EZSP_INTERFACE:
        if (strlen(optarg) > SERIAL_PORT_NAME_MAX_LEN) {
          fprintf(stderr, "Error: ezsp-interface name too long\n");
          exit(OT_EXIT_INVALID_ARGUMENTS);
        }
        strcpy(serialPort, optarg);
        break;
      case OT_POSIX_OPT_RADIO_URL:
        if (strlen(optarg) > RADIO_URL_MAX_LEN) {
          fprintf(stderr, "Error: Radio URL too long\n");
          exit(OT_EXIT_INVALID_ARGUMENTS);
        }
        strcpy(radioUrl, optarg);
        aConfig->mPlatformConfig.mRadioUrls[aConfig->mPlatformConfig.mRadioUrlNum++] = radioUrl;
        break;
      case OT_POSIX_OPT_VERBOSE:
        aConfig->mIsVerbose = true;
        break;
      case '?':
        fprintf(stderr, "Error: Unrecognized option (%s)\n", optarg);
        PrintUsage(aArgVector[0], stderr, OT_EXIT_INVALID_ARGUMENTS);
        break;
      default:
        assert(false);
        break;
    }
  }
}

void app_process_args(int argc, char *argv[])
{
  PosixConfig config = { 0 };
  config.mPlatformConfig.mSpeedUpFactor = 1;
  config.mLogLevel                      = OT_LOG_LEVEL_CRIT;

  int confArgc = 30;
  char *confArgv[confArgc + 1];
  for (int i = confArgc + 1; i; i--) {
    confArgv[i] = NULL;
  }
  confArgv[0] = argv[0];
  confArgc = ParseEtcConf("/usr/local/etc/zigbeed.conf", confArgc, confArgv + 1);
  if (confArgc < 0) {
    PrintUsage(argv[0], stderr, OT_EXIT_INVALID_ARGUMENTS);
  }
  ParseArg(confArgc + 1, confArgv, &config);
  for (int i = confArgc + 1; i; i--) {
    free(confArgv[i]);
  }

  ParseArg(argc, argv, &config);

  if (config.mPlatformConfig.mRadioUrlNum == 0) {
    fprintf(stderr, "RadioURL is undefined.\n");
    PrintUsage(argv[0], stderr, OT_EXIT_INVALID_ARGUMENTS);
  }

  openlog("zigbeed", LOG_PID | (config.mIsVerbose ? LOG_PERROR : 0), LOG_DAEMON);
  setlogmask(setlogmask(0) & LOG_UPTO(LOG_DEBUG));
  // Sleep added for successful restart of zigbeed when Host application resets
  sleep(1);
  otInstance *instance = otSysInit(&config.mPlatformConfig);
  IgnoreError(otLoggingSetLevel(config.mLogLevel));
  syslog(LOG_INFO, "Zigbeed started");
  syslog(LOG_INFO, "RCP version: %s", otPlatRadioGetVersionString(instance));
}
