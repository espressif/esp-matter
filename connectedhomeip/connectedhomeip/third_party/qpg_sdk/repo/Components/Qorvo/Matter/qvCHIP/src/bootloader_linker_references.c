
extern const int _binary_bl_userlicense_bin_start;
extern const int _binary_bootloader_bin_start;

const void *bl_keep_userlicense_hack = &_binary_bl_userlicense_bin_start;
const void *bl_keep_bootloader_hack = &_binary_bootloader_bin_start;
