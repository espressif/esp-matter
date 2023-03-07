# RT-Thread building script for component

from building import *

cwd = GetCurrentDir()
src = Glob('fastlz.c')
CPPPATH = [cwd]

if GetDepend('FASTLZ_USING_SAMPLE'):
    src += Glob('fastlz_sample.c')

group = DefineGroup('fastlz', src, depend = ['PKG_USING_FASTLZ'], CPPPATH = CPPPATH)

Return('group')
