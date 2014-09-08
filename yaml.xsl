<?xml version="1.0"  encoding="iso-8859-1" ?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">
    <xsl:output method="text"/>
    <xsl:include href="common.xsl"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>
    <xsl:template match="/*">
        <xsl:variable name="title">
            <xsl:apply-templates select="title"/>
        </xsl:variable>
        <xsl:text>---&#xa;title: "</xsl:text>
        <xsl:value-of select="normalize-space($title)"/>
        <xsl:text>"&#xa;---&#xa;</xsl:text>
    </xsl:template>
</xsl:stylesheet>
