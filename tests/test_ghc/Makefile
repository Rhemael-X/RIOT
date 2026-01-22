# name of your application
APPLICATION = test_ghc

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Include the necessary modules
USEMODULE += sixlowpan

# Add include path for ghc headers
CFLAGS += -I$(RIOTBASE)/include

# Add source files
SRC += test_ghc.c
SRC += $(RIOTBASE)/src/ghc/ghc.c
SRC += $(RIOTBASE)/src/ghc/ghc_cursor.c
SRC += $(RIOTBASE)/src/ghc/ghc_context.c
SRC += $(RIOTBASE)/src/ghc/ghc_ext.c
SRC += $(RIOTBASE)/src/ghc/ghc_ipv6.c

include $(RIOTBASE)/Makefile.include
