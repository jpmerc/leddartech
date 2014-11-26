# Makefile for Linux version of Leddar SDK.
# Copyrigth 2014 LeddarTech Inc.
# Modified by Jean-Philippe Mercier, Université Laval
include ../../Definitions.mif

Options = $(OptionsBase) -I../../Leddar -I../LeddarC
Solution = ../../Release/
Output = Release/
Module := $(Solution)LeddarCDemo

Base := Main
Base := $(Base:%=$(Output)%.o)

Tout := $(Base)

$(Module) : $(Output) $(Tout)
	gcc -o$(Module) -pthread -L../../Release $(Tout) -lLeddarC -lLeddar -lLeddarTech -lstdc++ -Wl,-rpath=.

$(Output)%.o : %.c Makefile ../../Definitions.mif
	$(Compiler) $< -o$@ $(Options)

$(Output) :
	mkdir -p $(Output)

-include $(Tout:.o=.d)
