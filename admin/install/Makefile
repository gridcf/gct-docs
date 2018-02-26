default: all

installingGT.pdf: index.txt
	$(TXT_TO_PDF)
installationAppendix.pdf: appendix.txt
	$(TXT_TO_PDF)

PDF_FILES   = installingGT.pdf installationAppendix.pdf
HTML_FILES  = index.html appendix.html

TOPDIR=../..
include $(TOPDIR)/rules.mk
