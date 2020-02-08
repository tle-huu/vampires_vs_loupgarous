env = Environment(
                    CXXFLAGS = '-std=c++17'
                )

VariantDir('build' , 'src', duplicate=0)

env.Program('client', Glob('build/*.cpp'))
