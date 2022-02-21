# ----------------------------
# Makefile Options
# ----------------------------

NAME = XPLORE
ICON = icon.png
DESCRIPTION = "An Exploration Game"
COMPRESSED = NO
ARCHIVED = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)
