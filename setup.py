from distutils.core import setup, Extension

module = [Extension('algo', 
                    sources=['algo.cpp'], 
                    include_dirs=["."], 
                    language="c++"
                   )
         ]

setup(name='algo', 
      ext_modules = module
     )
