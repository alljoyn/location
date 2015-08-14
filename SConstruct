import os
import platform
import re
import urlparse

#######################################################
# Custom Configure functions
#######################################################
def CheckCommand(context, cmd):
    context.Message('Checking for %s command...' % cmd)
    r = WhereIs(cmd)
    context.Result(r is not None)
    return r

def CheckAJLib(context, ajlib, ajheader, sconsvarname, ajdistpath, subdist, incpath, ext):
    prog = "#include <%s>\nint main(void) { return 0; }" % ajheader
    context.Message('Checking for AllJoyn library %s...' % ajlib)
    distpath = os.path.join(ajdistpath, subdist)
    prevLIBS = list(context.env.get('LIBS', []))
    prevLIBPATH = list(context.env.get('LIBPATH', []))
    prevCPPPATH = list(context.env.get('CPPPATH', []))

    # Check if library is in standard system locations
    context.env.Append(LIBS = [ajlib])
    defpath = ''  # default path is a system directory
    if not context.TryLink(prog, ext):
        # Check if library is in project default location
        context.env.Append(LIBPATH = os.path.join(distpath, 'lib'),
                           CPPPATH = os.path.join(distpath, incpath))
        if context.TryLink(prog, ext):
            defpath = str(Dir(ajdistpath))  # default path is the dist directory
        # Remove project default location from LIBPATH and CPPPATH
        context.env.Replace(LIBPATH = prevLIBPATH, CPPPATH = prevCPPPATH)

    vars = Variables()
    vars.Add(PathVariable(sconsvarname,
                          'Path to %s dist directory' % ajlib,
                          os.environ.get('AJ_%s' % sconsvarname, defpath),
                          lambda k, v, e : v == '' or PathVariable.PathIsDir(k, v, e)))
    vars.Update(context.env)
    Help(vars.GenerateHelpText(context.env))

    # Get the actual library path to use ('' == system path, may be same as distpath)
    libpath = context.env.get(sconsvarname, '')

    if libpath is not '':
        libpath = str(context.env.Dir(libpath))
        # Add the user specified (or distpath) to LIBPATH and CPPPATH
        context.env.Append(LIBPATH = os.path.join(libpath, subdist, 'lib'),
                           CPPPATH = os.path.join(libpath, subdist, incpath))

    # The real test for the library
    r = context.TryLink(prog, ext)
    if not r:
        context.env.Replace(LIBS = prevLIBS, LIBPATH = prevLIBPATH, CPPPATH = prevCPPPATH)
    context.Result(r)
    return r

def CheckAJCXXLib(context, ajlib, ajheader, sconsvarname, ajdistpath):
    r = CheckAJLib(context, ajlib, ajheader, sconsvarname, ajdistpath, 'cpp', 'inc', '.cc')
    return r

#######################################################
# Initialize our build environment
#######################################################
env = Environment(tools = ['default'])

Export('env', 'CheckAJCXXLib')

#######################################################
# Default target platform
#######################################################
if platform.system() == 'Linux':
    default_target = 'linux'
elif platform.system() == 'Windows':
    default_target = 'win32'
elif platform.system() == 'Darwin':
    default_target = 'darwin'

#######################################################
# Build variables
#######################################################
target_options = [ t.split('.')[-1] for t in os.listdir('.') if re.match('^SConscript\.target\.[-_0-9A-Za-z]+$', t) ]

vars = Variables()
vars.Add(BoolVariable('V',                  'Build verbosity',                     False))
vars.Add(EnumVariable('TARG',               'Target platform variant',             os.environ.get('AJ_TARG',               default_target), allowed_values = target_options))
vars.Add(EnumVariable('VARIANT',            'Build variant',                       os.environ.get('AJ_VARIANT',            'debug'),        allowed_values = ('debug', 'release')))
vars.Add('CC',  'C Compiler override')
vars.Add('CXX', 'C++ Compiler override')
vars.Update(env)
Help(vars.GenerateHelpText(env))

#######################################################
# Setup non-verbose output
#######################################################
if not env['V']:
    env.Replace( CCCOMSTR =          '\t[CC]       $SOURCE',
                 SHCCCOMSTR =        '\t[CC-SH]    $SOURCE',
                 CXXCOMSTR =         '\t[CXX]      $SOURCE',
                 SHCXXCOMSTR =       '\t[CXX-SH]   $SOURCE',
                 LINKCOMSTR =        '\t[LINK]     $TARGET',
                 SHLINKCOMSTR =      '\t[LINK-SH]  $TARGET',
                 JAVACCOMSTR =       '\t[JAVAC]    $SOURCE',
                 JARCOMSTR =         '\t[JAR]      $TARGET',
                 ARCOMSTR =          '\t[AR]       $TARGET',
                 ASCOMSTR =          '\t[AS]       $TARGET',
                 RANLIBCOMSTR =      '\t[RANLIB]   $TARGET',
                 INSTALLSTR =        '\t[INSTALL]  $TARGET',
                 WSCOMSTR =          '\t[WS]       $WS' )

#######################################################
# Load target setup
#######################################################
env['build'] = True
env['build_shared'] = False
env['build_unit_tests'] = True

env.SConscript('SConscript.target.$TARG')

env['CPU'] = platform.machine()

#######################################################
# Check dependencies
#######################################################
config = Configure(env, custom_tests = { 'CheckCommand' : CheckCommand,
                                           'CheckAJCXXLib' : CheckAJCXXLib })
found_ws = config.CheckCommand('uncrustify')

found_aj = config.CheckAJCXXLib('alljoyn',  'alljoyn/BusAttachment.h', 'ALLJOYN_DIST',
                                os.path.join('#../core/alljoyn/build', env['TARG'], env['CPU'], env['VARIANT'], 'dist'))
config.CheckCXXHeader('qcc/String.h')

env = config.Finish()

#######################################################
# Compilation defines
#######################################################
if env['VARIANT'] == 'release':
    env.Append(CPPDEFINES = [ 'NDEBUG' ])

if env['CPU'] == 'x86_64':
    env.Append(CPPDEFINES=['QCC_CPU_X86_64'])
else:
    env.Append(CPPDEFINES=['QCC_CPU_X86'])

#######################################################
# Include path
#######################################################
env.Append(CPPPATH = '#inc')

#######################################################
# Process commandline defines
#######################################################
env.Append(CPPDEFINES = [ v for k, v in ARGLIST if k.lower() == 'define' ])

#######################################################
# Install header files
#######################################################
env.Install('#dist/include/alljoyn/location', env.Glob('inc/alljoyn/location/*.h'))
# Need to force a dpendency here because SCons can't follow nested
# #include dependencies otherwise
env.Depends('#build/$VARIANT', '#dist/include')

#######################################################
# Build the various parts
#######################################################
env.SConscript('src/SConscript', variant_dir='#build/$VARIANT/src', duplicate = 0)
env.SConscript('samples/SConscript', variant_dir='#build/$VARIANT/samples', duplicate = 0)

#######################################################
# Run the whitespace checker
#######################################################
# Set the location of the uncrustify config file
if found_ws:
    import sys
    sys.path.append(os.getcwd() + '/tools')
    import whitespace

    def wsbuild(target, source, env):
        return whitespace.main([ env['WS'], os.getcwd() + '/tools/ajuncrustify.cfg' ])

    vars = Variables()
    vars.Add(EnumVariable('WS', 'Whitespace Policy Checker', os.environ.get('AJ_WS', 'check'), allowed_values = ('check', 'detail', 'fix', 'off')))

    vars.Update(env)
    Help(vars.GenerateHelpText(env))

    if env.get('WS', 'off') != 'off':
        env.Command('#ws', '#dist', Action(wsbuild, '$WSCOMSTR'))
