
#ifndef __RAIL_AUTOGEN_VER_DEF_H__
#define __RAIL_AUTOGEN_VER_DEF_H__

//=============================================================================
//
//  WARNING: Auto-Generated Git Info Header  -  DO NOT EDIT
//
//=============================================================================

#include <stdint.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
// Type Definitions
//-----------------------------------------------------------------------------

typedef enum
{
    GIT_TAG = 0,            // built from formal tag, pristine conditions
    GIT_DEV_BRANCH = 1,     // built from dev branch, not tagged and/or not
                            //   pristine conditions
    GIT_MASTER_BRANCH = 2   // built from master release branch, not tagged
                            //   and/or not pristine conditions
} GIT_INFO_LOCATION_e;

// Additional notes for details byte in FW_GIT_INFO_t:
//   DIRTY_FLAG: The combination of the index and the tracked files in the
//               working tree have changes with respect to HEAD.
//   LOCAL_COMMITS_FLAG: There are unpushed commits (ahead of the origin)
//   UNTRACKED_FILES_FLAG: Indicates untracked and not-ignored files found.

typedef struct
{
    uint32_t short_hash;  // first 32-bits of the git SHA-1 hash
    uint8_t  tag_major;   // tag major number    (<tag name>_major_minor_rev_build)
    uint8_t  tag_minor;   // tag minor number    (<tag name>_major_minor_rev_build)
    uint8_t  tag_rev;     // tag revision number (<tag name>_major_minor_rev_build)
    uint8_t  tag_build;   // tag revision number (<tag name>_major_minor_rev_build)
    uint8_t  details;     // (GIT_INFO_LOCATION_e << 4 |
                          //  UNTRACKED_FILES_FLAG << 2 |
                          //  LOCAL_COMMITS_FLAG << 1 | DIRTY_FLAG)
} FW_GIT_INFO_t;

#define GIT_INFO_SHORT_HASH (0x481e5817UL)
#define GIT_INFO_TAG_MAJOR (2)
#define GIT_INFO_TAG_MINOR (13)
#define GIT_INFO_TAG_REV (1)
#define GIT_INFO_TAG_BUILD (0)
#define GIT_INFO_DETAILS (0x00)

#endif  // __RAIL_AUTOGEN_VER_DEF_H__

