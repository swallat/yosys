
EXTRA_FLAG§S=
SEED=

ifneq ($(strip $(SEED)),)
SEEDOPT=-S$(SEED)
endif

$(MAKECMDGOALS):
	@$(basename $(MAKEFILE_LIST)).sh -G -j $(SEEDOPT) $(EXTRA_FLAGS) $@

.PHONY: $(MAKECMDGOALS)

