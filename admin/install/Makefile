default: all

installingGT.pdf: index.fo
installationAppendix.pdf: appendix.fo

appendixtarget.db: appendix.xml
target.db: index.xml

PDF_FILES   = installingGT.pdf installationAppendix.pdf
DB_FILES    = target.db appendixtarget.db
FO_FILES    = appendix.fo index-book.fo
HTML_FILES  = index.html appendix.html

TOPDIR=../..
include $(TOPDIR)/rules.mk
