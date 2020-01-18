MODULE := hal_input
ifneq (,$(filter xpt2046,$(USEMODULE)))
  SRC = hal_input_xpt2046.c
else
  SRC = hal_input_cst816s.c
endif
include $(RIOTBASE)/Makefile.base
