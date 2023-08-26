#!/usr/bin/env python

import os
from git.install_hooks import install_git_hooks

def install_submodules():
    print('Installing git submodules...')
    os.system('git submodule update --init --recursive')

if __name__ == '__main__':
    install_submodules()
    install_git_hooks()