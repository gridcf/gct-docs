<?xml version="1.0" encoding="iso-8859-1"?>
<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xi="http://www.w3.org/2001/XInclude" version="1.0">

  <xsl:output method="text"/>
  <xsl:param name="target"/>
  <xsl:param name="source"/>

  <xsl:template name="basename">
    <xsl:param name="str"/>
    <xsl:if test="contains($str, '/')">
      <xsl:call-template name="basename">
        <xsl:with-param name="str" select="substring-after($str, '/')"/>
      </xsl:call-template>
    </xsl:if>
    <xsl:if test="not(contains($str, '/'))">
      <xsl:value-of select="$str"/>
    </xsl:if>
  </xsl:template>

  <xsl:template name="dirname">
    <xsl:param name="path"/>
    <xsl:param name="previous-parts"/>
    <xsl:if test="contains($path, '/')">
      <xsl:call-template name="dirname">
        <xsl:with-param name="path" select="substring-after($path, '/')"/>
        <xsl:with-param name="previous-parts">
          <xsl:if test="$previous-parts">
            <xsl:value-of select="concat($previous-parts, '/', substring-before($path, '/'))"/>
          </xsl:if>
          <xsl:if test="not($previous-parts)">
            <xsl:value-of select="substring-before($path, '/')"/>
          </xsl:if>
        </xsl:with-param>
      </xsl:call-template>
    </xsl:if>
    <xsl:if test="not(contains($path, '/'))">
      <xsl:value-of select="$previous-parts"/>
    </xsl:if>
  </xsl:template>

  <xsl:template match="/*">
    <xsl:param name="target" select="$target"/>
    <xsl:param name="source" select="$source"/>
    <!--
    <xsl:message>
      <xsl:text>Parsing dependencies from </xsl:text>
      <xsl:value-of select="@id"/>
      <xsl:text> with source=</xsl:text>
      <xsl:value-of select="$source"/>
    </xsl:message>
    -->
    <xsl:value-of select="concat($target, ': ', $source,'&#10;')"/>
    <xsl:apply-templates>
        <xsl:with-param name="target" select="$target"/>
        <xsl:with-param name="source" select="$source"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template match="xi:include">
    <xsl:param name="target" select="$target"/>
    <xsl:param name="source" select="$source"/>
    <xsl:variable name="dirname">
        <xsl:choose>
            <xsl:when test="contains($source, '/')">
                <xsl:call-template name="dirname">
                    <xsl:with-param name="path" select="$source"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="'.'"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>

    <xsl:choose>
        <xsl:when test="@parse='text'">
            <xsl:value-of select="concat($target, ': ', $dirname, '/', @href, '&#10;')"/>
        </xsl:when>
        <xsl:otherwise>
            <xsl:apply-templates select="document(@href, .)/*">
              <xsl:with-param name="target" select="$target"/>
              <xsl:with-param name="source" select="concat($dirname, '/', @href)"/>
            </xsl:apply-templates>
        </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="glossary[@role='auto']">
    <xsl:value-of select="concat($target, ': ', $topdir, '/glossary.xml&#10;')"/>
  </xsl:template>
  <xsl:template match="*[@fileref]">
    <xsl:param name="target" select="$target"/>
    <xsl:param name="source" select="$source"/>

    <xsl:variable name="dirname">
        <xsl:choose>
            <xsl:when test="contains($source, '/')">
                <xsl:call-template name="dirname">
                    <xsl:with-param name="path" select="$source"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="'.'"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>

    <xsl:value-of select="
        concat($target, ': ', $dirname, '/', @fileref,'&#10;')"/>
  </xsl:template>

  <xsl:template match="text()"/>
</xsl:stylesheet>
