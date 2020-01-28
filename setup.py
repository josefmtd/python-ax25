#!/usr/bin/python3

from distutils.core import setup, Extension

module1 = Extension('pythonax25', libraries = ['ax25', 'ax25io'], sources = ['pythonax25.c'])

setup (name = 'pythonax25',
        version = '1.0',
        description = 'CPython extension for LINUX ax.25 stack',
        ext_modules = [module1])
