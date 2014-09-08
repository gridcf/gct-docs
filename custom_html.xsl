<?xml version="1.0"  encoding="iso-8859-1" ?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">
                
    <xsl:import href="http://docbook.sourceforge.net/release/xsl/current/html/onechunk.xsl"/>
    <xsl:param name="chunker.output.encoding" select="'ISO-8859-1'"/>
    <xsl:param name="draft.mode" select="'no'"></xsl:param>
    <xsl:include href="html.xsl"/>
    <xsl:include href="common.xsl"/>
</xsl:stylesheet>
