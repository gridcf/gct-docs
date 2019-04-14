default: all

PDF_FILES = index.pdf
HTML_FILES = index.html

SUBDIRS = 6.2

index.pdf: index.adoc
	$(ADOC_TO_PDF)


TOPDIR=.
include $(TOPDIR)/rules.mk
