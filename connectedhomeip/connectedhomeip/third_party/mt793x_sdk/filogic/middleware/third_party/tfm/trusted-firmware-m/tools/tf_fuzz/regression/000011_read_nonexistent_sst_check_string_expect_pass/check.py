#!/usr/bin/env python
# Copyright (c) 2019-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
import sys, os

def main():
    if len(sys.argv) != 4:
        print >> sys.stderr, "%s requires 3 command-line arguments. Exiting." % sys.argv[0]
        sys.exit(1)

    #print "Command line args: %s" % (', '.join(sys.argv[1:4]))
    sys.exit(0)

if __name__ == "__main__":
    main()
