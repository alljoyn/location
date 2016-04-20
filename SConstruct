import os
import platform
import glob

# Command arguments
os_arg = ARGUMENTS.get('OS', os.environ.get('OS'))
cpu_arg = ARGUMENTS.get('CPU', os.environ.get('CPU'))
cross_compile_arg = ARGUMENTS.get('CROSS_COMPILE', '')
variant_arg = ARGUMENTS.get('VARIANT', 'debug')
verbosity_arg = ARGUMENTS.get('V', '0')

# Variables
vars = Variables()

# Common build variables
vars.Add('OS', 'Target OS', os_arg)
vars.Add('CPU', 'Target CPU', cpu_arg)
vars.Add('CROSS_COMPILE', 'Toolchain Prefix', cross_compile_arg)
vars.Add('VARIANT', 'Build Variant', variant_arg)
vars.Add('V', 'Build Verbosity', verbosity_arg)

# Base environment
env = Environment(variables = vars, tools = ['gnulink', 'gcc', 'g++', 'ar', 'as', 'javac', 'javah', 'jar', 'pdf', 'pdflatex'])

# Specific build setup - update environment
Export('env')
SConscript('build_core/conf/' + env['OS'] + '/SConscript_' + env['CPU'])

# Variants - debug or release
if env['VARIANT'] == 'debug':
   env.Append(CFLAGS = '-g')
   env.Append(CXXFLAGS = '-g')
   env.Append(JAVACFLAGS = '-g')
else:
   env.Append(CFLAGS = '-Os')
   env.Append(CXXFLAGS = '-Os')
   env.Append(LINKFLAGS = ['-s', '-Wl,--gc-sections'])
   env.Append(CPPDEFINES = 'NDEBUG')



# Verbosity changes
if env['V'] == '0':
    env.Replace(CCCOMSTR =     '\t[CC]      $SOURCE',
                SHCCCOMSTR =   '\t[CC-SH]   $SOURCE',
                CXXCOMSTR =    '\t[CXX]     $SOURCE',
                SHCXXCOMSTR =  '\t[CXX-SH]  $SOURCE',
                LINKCOMSTR =   '\t[LINK]    $TARGET',
                SHLINKCOMSTR = '\t[LINK-SH] $TARGET',
                JAVACCOMSTR =  '\t[JAVAC]   $SOURCE',
                JARCOMSTR =    '\t[JAR]     $TARGET',
                ARCOMSTR =     '\t[AR]      $TARGET',
                RANLIBCOMSTR = '\t[RANLIB]  $TARGET'
                )

# Directory locations
dist_dir='dist/' + env['OS'] + '/' + env['CPU'] + '/' + env['VARIANT']
env.Replace(LIBPREFIX=[dist_dir + '/lib/lib'])

# Parse the argument list looking for additions
cpp_path_args = ['./']
lib_path_args = [dist_dir]
libs_args   = []
for key, value in ARGLIST:
    if key == 'CPPPATH':
        cpp_path_args.append(value)
    elif key == 'LIBPATH':
        lib_path_args.append(value)
    elif key == 'LIBS':
        libs_args.append(value)
env.Append(CPPPATH=cpp_path_args)
env.Append(LIBPATH=lib_path_args)
env.Append(LIBS=libs_args)

### BEGIN - THIS IS COMMENTED OUT BECAUSE WE ARE NOT CURRENTLY USING THE CODE GENERATOR
# Generated sources
#def gen_files(env, gen_type, src_file, dst_dir ) :
#    if not os.path.isdir(dst_dir):
#        os.mkdir(dst_dir)
#        os.system('ajcodegen.py -w org.alljoyn.locationservices' + \
#                  ' -p ' + dst_dir + \
#                  ' -t ' + gen_type + \
#                  '    ' + src_file)
#    return glob.glob(dst_dir + "/*")
#env.AddMethod(gen_files, 'GenFiles')
#generated_cpp=env.GenFiles("ddcpp",   "locationservices.xml", "locationservices_cpp")
#generated_c=env.GenFiles("tl",      "locationservices.xml", "locationservices_c")
#generated_android=env.GenFiles("android", "locationservices.xml", "locationservices_android")
#
## CPP provider/consumer library builds
#generated_cpp_src = filter(lambda f: not '.h' in f, generated_cpp)
#generated_cpp_cons = filter(lambda f: not 'Interface' in f, generated_cpp_src)
#generated_cpp_prov = filter(lambda f: not 'Proxy' in f, generated_cpp_src)
#
#cons_static=env.StaticLibrary("consumer", generated_cpp_cons)
#prov_static=env.StaticLibrary("provider", generated_cpp_prov)
### END - THIS IS COMMENTED OUT BECAUSE WE ARE NOT CURRENTLY USING THE CODE GENERATOR


# Install location services include files.
env.Install([dist_dir + '/inc/ls/'], ['./LsStd.h','./LocationServices.h',
                                      './includes/PresenceManagerInterface.h',
                                      './includes/PresenceTrackerInterface.h',
                                      './includes/PresenceContributorInterface.h',
                                      './includes/DistanceManagerInterface.h',
                                      './includes/DistanceTrackerInterface.h',
                                      './includes/DistanceContributorInterface.h',
                                      './includes/GeofenceManagerInterface.h',
                                      './includes/GeofenceTrackerInterface.h',
                                      './includes/PositionManagerInterface.h',
                                      './includes/PositionTrackerInterface.h',
                                      './includes/PositionContributorInterface.h'])


# lservices Application (updated interfaces)
lservices_list=env.Glob('lservices/*.cc') 
lservices_app=env.Program(target='lservices/lservices', source=lservices_list, CXXFLAGS=['$CXXFLAGS', '-fexceptions'], LINKFLAGS=['$LINKFLAGS', '-Wl,-Map=./lservices/lservices.map'])
env.Install([dist_dir + '/'], lservices_app)


