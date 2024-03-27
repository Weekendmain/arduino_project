#!/usr/bin/env python3

# Copyright (C) 2021 Lee C. Bussy (@LBussy)

# This file is part of Lee Bussy's mDNSResolver.

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import subprocess
import os
import sys
import fileinput
import json
Import("env")

# Get Git project name
project = None
try:
    projcmd = "git rev-parse --show-toplevel"
    project = subprocess.check_output(projcmd, shell=True).decode().strip()
    project = project.split("/")
    project = project[len(project)-1]
except:
    project = os.path.dirname(os.path.realpath(__file__))
    project = project.split("/")
    project = project[len(project)-1]
if not project or project == "":
    project = "unknown"

# Get 0.0.0 version from latest Git tag
version = None
try:
    tagcmd = "git describe --tags --abbrev=0"
    version = subprocess.check_output(tagcmd, shell=True).decode().strip()
except:
    pass
if not version or version == "":
    version = "0.0.0"

# Get latest commit short from Git
commit = None
try:
    revcmd = "git rev-parse --short HEAD"
    commit = subprocess.check_output(revcmd, shell=True).decode().strip()
except:
    pass
if not commit or commit == "":
    commit = "0123456"

# Get branch name from Git
branch = None
try:
    branchcmd = "git rev-parse --abbrev-ref HEAD"
    branch = subprocess.check_output(branchcmd, shell=True).decode().strip()
except:
    pass
if not branch or branch == "":
    branch = "devel"

for line in fileinput.input(str(env["PROJECT_SRC_DIR"]) + '/mDNSResolver.h', inplace=True):
    if line.strip().startswith('#define PIO_SRC_NAM'):
        line = '#define PIO_SRC_NAM "{}"\n'.format(project)
    if line.strip().startswith('#define PIO_SRC_TAG'):
        line = '#define PIO_SRC_TAG "{}"\n'.format(version)
    if line.strip().startswith('#define PIO_SRC_BRH'):
        line = '#define PIO_SRC_BRH "{}"\n'.format(branch)
    if line.strip().startswith('#define PIO_SRC_REV'):
        line = '#define PIO_SRC_REV "{}"\n'.format(commit)
    sys.stdout.write(line)

# Read in: 'library.json'
# Replace: "version": "0.0.1"
library_json = 'library.json'
with open(library_json) as json_file:
    data = json.load(json_file)
    data['version'] = version
    with open(library_json, 'w') as outfile:
        json.dump(data, outfile, ensure_ascii=False, indent=4)  

for line in fileinput.input('library.properties', inplace=True):
    if line.strip().startswith('version='):
        line = 'version={}\n'.format(version)
    sys.stdout.write(line)
