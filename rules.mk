parentdir=.

# Allow local overrides if wanted
ifneq ($(wildcard $(TOPDIR)/local.mk),)
include $(TOPDIR)/local.mk
endif

# default html file if not specified
ifeq (,$(HTML_FILES))
    HTML_FILES = index.html
endif

CLEANFILES = $(HTML_FILES) $(PDF_FILES)

TXT_TO_PDF = asciidoctor-pdf -d book -o $@ $<
TXT_TO_HTML = asciidoctor -d book -o $@ $<
CONCAT_PDFS = gs -q -sPAPERSIZE=letter -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -dPDFSETTINGS=/prepress -sOutputFile=$@ $(filter %.pdf,$^)

# default rule
all: html pdf

html: html-recursive html-local
html-local: $(HTML_FILES)
pdf: pdf-recursive pdf-local
pdf-local: $(PDF_FILES)
clean: clean-recursive clean-local
clean-local:
	@for f in $(CLEANFILES); do [ -f "$$f" ] && rm "$$f" || true; done

html-recursive pdf-recursive clean-recursive:
	@if [ "$(SUBDIRS)" != "" ]; then \
            for dir in $(SUBDIRS); do echo "Entering $(parentdir)/$$dir [$(subst -recursive,,$@])" ; $(MAKE) -C $$dir parentdir=$(parentdir)/$$dir $(subst -recursive,,$@) || exit 1; done \
        fi

%.html: %.txt
	$(TXT_TO_HTML)

%.pdf:
	@if expr $< : '.*.txt' > /dev/null; then \
		$(TXT_TO_PDF); \
	else \
		$(CONCAT_PDFS); \
	fi

$(SUBDIRS):
	@make -C $@

.SUFFIXES: .pdf .txt
.PHONY: all clean-recursive clean pdf $(SUBDIRS) html-local pdf-local
