<?xml version="1.0"?>
<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:exsl="http://exslt.org/common"
    exclude-result-prefixes="exsl"
    extension-element-prefixes="exsl"
    version="1.0" >

    <xsl:param name="indent-level" select="'-1'"/>
    <xsl:key name="program-xrefs" match="para|formalpara|section" use="@id"/>
    <xsl:output method="text"/>
    <xsl:template match="text()"/>

    <!-- matches a top-level program listing -->
    <xsl:template match="programlisting[@role]">
        <xsl:param name="indent-level" select="$indent-level"/>
        <exsl:document method="text" href="{@role}" indent="no">
            <xsl:apply-templates mode="output-program">
                <xsl:with-param name="indent-level" select="'-1'"/>
            </xsl:apply-templates>
        </exsl:document>
    </xsl:template>
    
    <xsl:template match="programlisting" mode="output-program">
        <xsl:param name="indent-level" select="$indent-level"/>

        <xsl:apply-templates mode="output-program">
            <xsl:with-param name="indent-level" select="$indent-level"/>
        </xsl:apply-templates>
    </xsl:template>


    <xsl:template name="string-replace-all">
        <xsl:param name="text"/>
        <xsl:param name="replace"/>
        <xsl:param name="by"/>
        <xsl:choose>
            <xsl:when test="contains($text, $replace)">
                <xsl:value-of select="substring-before($text,$replace)"/>
                <xsl:value-of select="$by"/>
                <xsl:call-template name="string-replace-all">
                    <xsl:with-param name="text" select="substring-after($text,$replace)"/>
                    <xsl:with-param name="replace" select="$replace"/>
                    <xsl:with-param name="by" select="$by"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="$text"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template match="text()" mode="output-program">
        <xsl:param name="indent-level" select="$indent-level"/>
        <xsl:variable name="indent">
            <xsl:call-template name="indent-text">
                <xsl:with-param name="indent-level" select="$indent-level"/>
            </xsl:call-template>
        </xsl:variable>
        <xsl:call-template name="string-replace-all">   
            <xsl:with-param name="text" select="."/>
            <xsl:with-param name="replace" select="'&#10;'"/>
            <xsl:with-param name="by" select="concat('&#10;', $indent)"/>
        </xsl:call-template>
        <xsl:value-of select="'&#10;'"/>
    </xsl:template>

    <xsl:template match="xref" mode="output-program">
        <xsl:param name="indent-level" select="$indent-level"/>

        <xsl:variable name="my-indent-level">
            <xsl:choose>
                <xsl:when test="@role = 'noindent'">
                    <xsl:value-of select="$indent-level"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="$indent-level + 1"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:variable name="indent">
            <xsl:call-template name="indent-text">
                <xsl:with-param name="indent-level" select="$my-indent-level"/>
            </xsl:call-template>
        </xsl:variable>

        <xsl:variable name="label">
            <xsl:choose>
                <xsl:when test="key('program-xrefs', @linkend)/@xreflabel">
                    <xsl:value-of select="key('program-xrefs', @linkend)/@xreflabel"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="@linkend"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:value-of select="concat($indent, '/* ', $label, ' */&#10;', $indent)"/>
        <xsl:apply-templates select="
                key('program-xrefs', @linkend)/*[name() = 'para' or name() = 'formalpara']/programlisting"
                mode="output-program">
            <xsl:with-param name="indent-level" select="$my-indent-level"/>
        </xsl:apply-templates>
    </xsl:template>

    <xsl:template name="indent-text">
        <xsl:param name="indent-level"/>

        <xsl:if test="$indent-level > 0">
            <xsl:text>    </xsl:text>
            <xsl:call-template name="indent-text">
                <xsl:with-param name="indent-level" select="$indent-level - 1"/>
            </xsl:call-template>
        </xsl:if>
    </xsl:template>

    <xsl:template match="*" mode="output-program"/>
</xsl:stylesheet>
