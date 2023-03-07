/***************************************************************************//**
 * @file
 * @brief This code handles Linux specifics related to generating a cryptographic
 * quality random number.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include PLATFORM_HEADER

#include <stdio.h>  // fread
#include <errno.h>  // errno
#include <stdlib.h> // rand
#include "random-number-generation.h"

//==============================================================================
// Globals
//==============================================================================

// /dev/random is supposed to provide random numbers suitable for security
// operations.  It is assumed that most Linux distributions save a random
// number on shutdown so that it can be used as the seed for the random number
// generator on bootup.  However we can't guarantee that it is the best source.
// Our strategy is print out the source for the randomness and hope security
// conscious users will verify this on their own systems.
#ifndef EMBER_TEST
static const char randomDevice[] = "/dev/random";
#endif
//==============================================================================
// Functions
//==============================================================================

#ifdef EMBER_TEST
int platformRandomDataFunction(unsigned char* buffer, unsigned long size)
{
  unsigned int i;
  for (i = 0; i < size; i++) {
    buffer[i] = (rand() & 0xFF);
  }

  return 0;
}

#else

int platformRandomDataFunction(unsigned char* buffer, unsigned long size)
{
  int status = 0;

  fprintf(stderr, "Using %s for random number generation\n", randomDevice);
  FILE* randDevice = fopen(randomDevice, "r");
  if (randDevice == NULL) {
    fprintf(stderr, "Could not open random device: %s\n", strerror(errno));
    return 1;
  }
  fprintf(stderr, "Gathering sufficient entropy... (may take up to a minute)...\n");

  if (size != fread(buffer,
                    1,       // size of each element
                    size,    // number of elements
                    randDevice)) {
    fprintf(stderr, "Failed to read random data from device: %s\n", strerror(errno));
    status = 1;
  }
  fclose(randDevice);
  return status;
}
#endif
