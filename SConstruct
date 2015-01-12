Debug = True
Release = not Debug

env = Environment()
env.Append(CPPPATH = ["#"])
env.Append(CCFLAGS = ["-std=c++0x"])  # C++11 support
if (Release): 
    env.Append(CPPDEFINES = ["NDEBUG"])

Export("env")

SConscript("common/SConscript")
SConscript("samples/SConscript")
SConscript("algorithm/SConscript")
SConscript("recsys/SConscript")
SConscript("cv/SConscript")
