from distutils.core import setup, Extension

module = Extension('algo', 
                   sources = ['algo.cpp', 
                              'src/utils.cpp', 
                              'src/Point.cpp', 
                              'src/Group.cpp', 
                              'src/Map.cpp', 
                              'src/Action.cpp', 
                              'src/State.cpp', 
                              'src/minimax.cpp'
                             ], 
                   include_dirs = ['include'], 
                   language = 'c++'
                  )

setup(name = 'algo', 
      version = '1.0', 
      description = 'Package to have an IA playing Vampires versus Werewolves', 
      author = 'Elie Mauffrey', 
      author_email = 'emauffrey@yahoo.fr', 
      ext_modules = [module]
     )
