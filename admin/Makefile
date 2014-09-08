default: all
LOCAL_XSLTPROC_HTML_PARAMS=--stringparam generate.toc ""


PDF_FILES = gtAdmin.pdf
gtAdmin.pdf: \
    quickstart/gtQuickstart.pdf \
    install/installingGT.pdf \
    install/installationAppendix.pdf 

SUBDIRS=install quickstart

TOPDIR=..
include $(TOPDIR)/rules.mk
