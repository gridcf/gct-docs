default: all

installingGT.pdf: index.adoc
	$(ADOC_TO_PDF)
installationAppendix.pdf: appendix.adoc
	$(ADOC_TO_PDF)

PDF_FILES   = installingGT.pdf installationAppendix.pdf
HTML_FILES  = index.html appendix.html

TOPDIR=../..
include $(TOPDIR)/rules.mk
