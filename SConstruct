env = Environment()
env.Append(CPPPATH = ["#"])
env.Append(CCFLAGS = ["-std=c++0x"])  # add C++11 support
Export("env")

SConscript("common/SConscript")
SConscript("samples/SConscript")
SConscript("algorithm/SConscript")
SConscript("recsys/SConscript")
