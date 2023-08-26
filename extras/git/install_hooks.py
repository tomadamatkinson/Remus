#!/usr/bin/env python

import os
import sys
import shutil

def install_git_hooks():
    # Get the path to the git hooks directory
    local_git_hooks = os.path.join(os.path.dirname(__file__), 'hooks')
    target_git_hooks = os.path.join(os.path.dirname( __file__), '..', '..', '.git', 'hooks')

    # Copy all the files from the local git hooks directory to the target directory
    for filename in os.listdir(local_git_hooks):
        source = os.path.join(local_git_hooks, filename)
        target = os.path.join(target_git_hooks, filename)
        shutil.copyfile(source, target)

    # Make sure the hooks are executable
    for filename in os.listdir(target_git_hooks):
        target = os.path.join(target_git_hooks, filename)
        os.chmod(target, 755)

    # Print a message to the user
    print("Git hooks installed in %s" % target_git_hooks)

if __name__ == '__main__':
    install_git_hooks()