#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/brock/dev/projects/CryptoMidterm/build
  make -f /Users/brock/dev/projects/CryptoMidterm/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/brock/dev/projects/CryptoMidterm/build
  make -f /Users/brock/dev/projects/CryptoMidterm/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/brock/dev/projects/CryptoMidterm/build
  make -f /Users/brock/dev/projects/CryptoMidterm/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/brock/dev/projects/CryptoMidterm/build
  make -f /Users/brock/dev/projects/CryptoMidterm/build/CMakeScripts/ReRunCMake.make
fi

