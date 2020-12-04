

TOPDIR  = ../CAFE2p0
TOPDIRIN= .
SRCDIR  = $(wildcard $(TOPDIR)/Afe79xx/Src)
SRCDIR += $(wildcard $(TOPDIRIN)/Afe79xxTI/Src)
OBJDIR  = $(shell mkdir -p Obj; ls -d Obj)
INCDIR1 = $(wildcard $(TOPDIR)/Afe79xx/Include)
INCDIR2 = $(wildcard $(TOPDIRIN)/Afe79xxTI/Include)



SRCFILES = $(foreach var,$(SRCDIR),$(shell ls -d $(var)/*.c))
OBJFILES = $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(notdir $(SRCFILES))))
CC = gcc

CFLAGS = -Wall -Wextra -g
IFLAGS = -I$(INCDIR1) -I$(INCDIR2)

LINKDIR = $(wildcard $(TOPDIRIN)/Afe79xxTI)
LFLAGS= -L$(LINKDIR)

TARGET = ./Afe79xxTI/libCAFE.dll

VPATH = $(SRCDIR)


$(TARGET):$(OBJFILES)
	$(CC) -lm -shared -o $@ $^ $(LFLAGS) -linterface -lwrapper

$(OBJDIR)/%.o:%.c
	$(CC) $(CFLAGS) $(IFLAGS) -o $@ -c $<

clean:
	@rm -rf $(OBJDIR)
	@rm -rf $(TARGET)

debug:
	@echo "[TOPDIR  ][$(TOPDIR)]"
	@echo "[TOPDIRIN  ][$(TOPDIRIN)]"
	@echo "[SRCDIR  ][$(SRCDIR)]"
	@echo "[OBJDIR  ][$(OBJDIR)]"
	@echo "[INCDIR1  ][$(INCDIR1)]"
	@echo "[INCDIR2  ][$(INCDIR2)]"
	@echo "[SRCFILES][$(SRCFILES)]"
	@echo "[OBJFILES][$(OBJFILES)]"
	@echo "[CC      ][$(CC)]"
	@echo "[CFLAGS  ][$(CFLAGS)]"
	@echo "[IFLAGS  ][$(IFLAGS)]"
	@echo "[VPATH   ][$(VPATH)]"
