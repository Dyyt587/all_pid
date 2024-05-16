Import('rtconfig')
from building import *
import os

cwd = GetCurrentDir()
src = Glob('src/*.c')
inc = [cwd+"/src"]

CXXFLAGS = ''

group = DefineGroup('apid', src, depend = ['PKG_USING_APID'], CPPPATH = inc, CXXFLAGS = CXXFLAGS)

Return('group')
