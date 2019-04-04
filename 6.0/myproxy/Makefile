default: all

PDF_FILES = myproxy.pdf
SUBDIRS=admin developer mig pi qp rn user

PDFS=admin/myproxyAdminGuide.pdf user/myproxyUserGuide.pdf developer/myproxyDeveloperGuide.pdf mig/myproxyMigratingGuide.pdf qp/myproxyQualityProfile.pdf rn/myproxyReleaseNotes.pdf

myproxy.pdf: $(PDFS)

TOPDIR=..
include $(TOPDIR)/rules.mk

