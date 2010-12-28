#!/bin/sh

# run gtkdocize
gtkdocize || exit 1

# gnome-autogen.sh runs configure, so do likewise.
autoreconf -vif && ./configure "$@"
