#!/usr/bin/env python

# Recursively collect all dynamic lib dependencies of
# all executables in the current working directory.

import subprocess
import re
import shutil
import os.path
import os

lib_set = set([])
lib_set_old = None
while lib_set != lib_set_old:
    lib_set_old = lib_set
    lib_set = set([])
    for dirpath, _, files in os.walk("."):
        for i in files:
            i = dirpath + '/' + i
            print "Checking " + i
            try:
                str = subprocess.check_output(['ldd', i])
            except:
                continue
            for j in str.split('\n'):
                #       libcrypt.so.1 => /lib/x86_64-linux-gnu/libcrypt.so.1 (0x00007fa4be1c1000)
                m = re.match(r'\s*(.+) => (not found|[^ ]+).*', j)
                if m == None : continue
                path = m.group(2)
                if path == "not found":
                    print "Fatal error: " + m.group(1) + " not found"
                    exit(1)
                name = os.path.basename(path)
                # skip libdl, libc
                if re.search(r'^lib(dl|c\.)', name) : continue
                lib_set.add(name)
                if not os.path.exists(name):
                    print "Copying " + name
                    shutil.copy(path, name)
