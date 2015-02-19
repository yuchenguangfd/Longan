Debug = True
CompileOptimize = True

env = Environment()
env.Append(CPPPATH = ["#"])
env.Append(CCFLAGS = ["-std=c++0x"])  # C++11 support
if not Debug: 
    env.Append(CPPDEFINES = ["NDEBUG"])
if CompileOptimize:
    env.Append(CCFLAGS = ["-O3", "-march=native", "-funroll-loops"])

Export("env")

SConscript("common/SConscript")
SConscript("samples/SConscript")
SConscript("algorithm/SConscript")
SConscript("recsys/SConscript")
SConscript("ml/SConscript")
SConscript("cv/SConscript")
