# define default target before pdf dependencies
default: all

PDF_FILES   = gtQuickstart.pdf
gtQuickstart.pdf: index.adoc
	asciidoctor-pdf -d book -o $@ $<

TOPDIR=../..
include $(TOPDIR)/rules.mk
