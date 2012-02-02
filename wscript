import Options
from os.path import exists
from shutil import copy2 as copy

TARGET = 'hogan'
TARGET_FILE = '%s.node' % TARGET
built = 'build/Release/%s' % TARGET_FILE
dest = 'lib/hogan.jit/%s' % TARGET_FILE

def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")

def pre(ctx):
  if Options.platform == 'darwin' and ctx.env['DEST_CPU'] == 'i386':
    ctx.exec_command('make -B ARCH=i386 MODE=RELEASE -C ../deps/hogan.jit/')
  else:
    ctx.exec_command('make -B ARCH=x64 MODE=RELEASE -C ../deps/hogan.jit/')

def build(bld):
  bld.add_pre_fun(pre)
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.cxxflags = ["-g", "-D_LARGEFILE_SOURCE", "-Wall"]
  obj.ldflags = ["../deps/hogan.jit/hogan.a"]
  obj.target = TARGET
  obj.source = "src/node_hogan.cc"
  obj.includes = "src/ deps/hogan.jit/include"

def shutdown():
  if Options.commands['clean']:
      if exists(TARGET_FILE):
        unlink(TARGET_FILE)
  else:
    if exists(built):
      copy(built, dest)
