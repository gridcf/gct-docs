default: all

PDF_FILES = index.pdf
SUBDIRS= \
admin       gram5    gsiopenssh  simpleca \
appendices  gridftp  myproxy     xio \
ccommonlib  gsic     rn

index.pdf: index.fo
FO_FILES = target.db
target.db: index.xml

TOPDIR=.
include $(TOPDIR)/rules.mk
