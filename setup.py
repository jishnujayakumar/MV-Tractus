from distutils.core import setup, Extension

module1 = Extension('efmvs',
                    sources = ['efmvs.cpp'])

setup (name = 'FMV',
       version = '1.0',
       description = 'This is a demo package')
