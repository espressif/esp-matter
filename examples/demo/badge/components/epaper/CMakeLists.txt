set(COMPONENT_SRCS "epaper.c"
                   "epaper_font.c"
                   "imagedata.c"
                   "lowpower_evb_epaper.cpp")
set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_REQUIRES driver qrcode)

register_component()
