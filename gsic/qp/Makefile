default: all

gsicQualityProfile.pdf: index.adoc
PDF_FILES = gsicQualityProfile.pdf

%-test.html: %-test.xml
	xsltproc junit.xsl $< > $@

TOPDIR=../..
include $(TOPDIR)/rules.mk
