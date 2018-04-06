# Project configs
REPORT := report
SLIDES := slides
BIBREF := references.bib

TARGETS := $(REPORT) $(SLIDES)

# Compiler configs
LATEX  := xelatex
BIBTEX := bibtex

# Etc.
OBJS   := $(wildcard *.tex)

# Rules
.PHONY: clean clean-pdf slides all

all: $(TARGETS)

report: $(REPORT).pdf
slides: $(SLIDES).pdf

%.pdf: $(OBJS) $(BIBREF)
	$(LATEX) --shell-escape $(basename $@)
ifdef BIBREF
	$(BIBTEX) $(basename $@)
	$(LATEX) --shell-escape $(basename $@)
	$(LATEX) --shell-escape $(basename $@)
endif

clean:
	rm -f *.{aux,bbl,blg,log,nav,snm,out,toc}
	rm -f *.{hi,o}
	rm -rf _minted-*

clean-pdf:
	rm $(BRIEF).pdf $(SLIDES).pdf
