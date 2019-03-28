default: all


PDF_FILES = gtAdmin.pdf
gtAdmin.pdf: \
    quickstart/gtQuickstart.pdf \
    install/installingGT.pdf \
    install/installationAppendix.pdf 
	$(CONCAT_PDFS)

SUBDIRS=install quickstart

TOPDIR=..
include $(TOPDIR)/rules.mk
