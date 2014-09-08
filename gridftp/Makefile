default: all

SUBDIRS= admin developer key mig pi qp rn user

gridftp.pdf: \
    admin/gridftpAdminGuide.pdf \
    user/gridftpUserGuide.pdf \
    developer/gridftpDeveloperGuide.pdf \
    mig/gridftpMigratingGuide.pdf \
    qp/gridftpQualityProfile.pdf \
    rn/gridftpReleaseNotes.pdf

PDF_FILES = gridftp.pdf

TOPDIR=..
include $(TOPDIR)/rules.mk
