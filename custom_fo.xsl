<?xml version="1.0"  encoding="iso-8859-1" ?>
<xsl:stylesheet
 xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
 xmlns:fo="http://www.w3.org/1999/XSL/Format"
 version="1.0">
 
 <!-- now replace all these settings with those specific for use with the fo stylesheet (for pdf output) -->
               <!-- which stylesheet to use? -->
               <xsl:import href="http://docbook.sourceforge.net/release/xsl/current/fo/docbook.xsl"/>
               <xsl:include href="common.xsl"/>
               
                
<xsl:param name="collect.xref.targets">yes</xsl:param>

               <!-- allow fragment identifiers in pdf? -->
               <xsl:param name="insert.olink.pdf.frag" select="1"></xsl:param>
               
               <!-- making olinks underlined in pdf output -->
               <xsl:attribute-set name="olink.properties">
                              <xsl:attribute name="text-decoration">underline</xsl:attribute>
               </xsl:attribute-set>
               
               <!-- default location for target database document (for olinks) -->
               <xsl:param name="target.database.document">olinkdb.xml</xsl:param>
               
               <!-- enable extensions -->
               <xsl:param name="xep.extensions" select="1"></xsl:param>
               
               <!-- turn off table column extensions (unless you use xalan or saxon - it's a java thing -->
               <xsl:param name="tablecolumns.extension" select="'0'"></xsl:param>
               
               <!-- should output be in draft mode? -->
               <xsl:param name="draft.mode" select="'no'"></xsl:param>
               <xsl:param name="draft.watermark.image"><xsl:value-of select="$topdir"/>/docbook/xsl/docbook-xsl-1.78.1/images/draft.png</xsl:param>
               
               <!-- ALIGNMENT -->
               <xsl:param name="alignment">left</xsl:param>
               
               <!-- GRAPHICS -->
               <!-- Set path to admonition graphics  -->
               <xsl:param name="admon.graphics.path"><xsl:value-of select="$topdir"/>/docbook/xsl/docbook-xsl-1.78.1/images/</xsl:param>
                
                <!-- Set path to callout graphics -->
                <xsl:param name="callout.graphics.path"><xsl:value-of select="$topdir"/>/docbook/xsl/docbook-xsl-1.78.1/images/</xsl:param>
               
               <!-- Again, if 1 above, what is the filename extension for admon graphics?-->
                <xsl:param name="admon.graphics.extension" select="'.png'"/> 
               
               <!-- for some reason, xep makes the admon graphics too large, this scales them back down -->
               <xsl:template match="*" mode="admon.graphic.width">14pt</xsl:template>
                
               <!-- callouts look fuzzy in print - using the following two parameters to force unicode -->
               <xsl:param name="callout.graphics" select="'0'"></xsl:param>
               
               <xsl:param name="callout.unicode" select="1"></xsl:param>
               
               <!-- NUMBERING -->

               <!-- are parts enumerated?  COMMON -->
                <xsl:param name="part.autolabel">1</xsl:param>
                
               <!-- how deep should each toc be? (how many levels?) COMMON -->
                <xsl:param name="toc.max.depth">2</xsl:param>
                
               <!-- How deep should recursive sections appear in the TOC? COMMON -->
                <xsl:param name="toc.section.depth">1</xsl:param>
               
               <!-- LINKS -->
               
               <!-- display ulinks as footnotes at bottom of page? -->
               <xsl:param name="ulink.footnotes" select="1"></xsl:param>
               
               <!-- display xref links with underline? -->
               <xsl:attribute-set name="xref.properties">
                              <xsl:attribute name="text-decoration">underline</xsl:attribute> 
               </xsl:attribute-set>
               
               <!-- TABLES -->
               
               <xsl:param name="default.table.width" select="'6in'"></xsl:param>
              

                <!-- INDEX  -->
               
               <!-- index attributes for xep -->
               <xsl:attribute-set name="xep.index.item.properties">
                              <xsl:attribute name="merge-subsequent-page-numbers">true</xsl:attribute>
                              <xsl:attribute name="link-back">true</xsl:attribute>
               </xsl:attribute-set>
                
               <!-- reduce 'indentation' of body text -->
               <xsl:param name="body.start.indent">
                              <xsl:choose>
                                             <xsl:when test="$fop.extensions != 0">0pt</xsl:when>
                                             <xsl:when test="$passivetex.extensions != 0">0pt</xsl:when>
                                             <xsl:otherwise>0pc</xsl:otherwise>
                              </xsl:choose>
               </xsl:param>
                
                 <!-- try to add titleabbrev to part -->
               <xsl:template match="part">
                              <xsl:if test="not(partintro)">
                                             <xsl:apply-templates select="." mode="part.titlepage.mode"/>
                                             <xsl:call-template name="generate.part.toc"/>
                              </xsl:if>
                              <xsl:apply-templates/>
               </xsl:template>
               
               <xsl:template match="part" mode="part.titlepage.mode">
                              <!-- done this way to force the context node to be the part -->
                              <xsl:param name="additional.content"/>
                              
                              <xsl:variable name="id">
                                             <xsl:call-template name="object.id"/>
                              </xsl:variable>
                              
                              <xsl:variable name="titlepage-master-reference">
                                             <xsl:call-template name="select.pagemaster">
                                                            <xsl:with-param name="pageclass" select="'titlepage'"/>
                                             </xsl:call-template>
                              </xsl:variable>
                              
                              <fo:page-sequence hyphenate="{$hyphenate}"
                                             master-reference="{$titlepage-master-reference}">
                                             <xsl:attribute name="language">
                                                            <xsl:call-template name="l10n.language"/>
                                             </xsl:attribute>
                                             <xsl:attribute name="format">
                                                            <xsl:call-template name="page.number.format">
                                                                           <xsl:with-param name="master-reference"
                                                                                          select="$titlepage-master-reference"/>
                                                            </xsl:call-template>
                                             </xsl:attribute>
                                             
                                             <xsl:attribute name="initial-page-number">
                                                            <xsl:call-template name="initial.page.number">
                                                                           <xsl:with-param name="master-reference"
                                                                                          select="$titlepage-master-reference"/>
                                                            </xsl:call-template>
                                             </xsl:attribute>
                                             
                                             <xsl:attribute name="force-page-count">
                                                            <xsl:call-template name="force.page.count">
                                                                           <xsl:with-param name="master-reference"
                                                                                          select="$titlepage-master-reference"/>
                                                            </xsl:call-template>
                                             </xsl:attribute>
                                             
                                             <xsl:attribute name="hyphenation-character">
                                                            <xsl:call-template name="gentext">
                                                                           <xsl:with-param name="key" select="'hyphenation-character'"/>
                                                            </xsl:call-template>
                                             </xsl:attribute>
                                             <xsl:attribute name="hyphenation-push-character-count">
                                                            <xsl:call-template name="gentext">
                                                                           <xsl:with-param name="key" select="'hyphenation-push-character-count'"/>
                                                            </xsl:call-template>
                                             </xsl:attribute>
                                             <xsl:attribute name="hyphenation-remain-character-count">
                                                            <xsl:call-template name="gentext">
                                                                           <xsl:with-param name="key" select="'hyphenation-remain-character-count'"/>
                                                            </xsl:call-template>
                                             </xsl:attribute>
                                             
                                             <xsl:apply-templates select="." mode="running.head.mode">
                                                            <xsl:with-param name="master-reference" select="$titlepage-master-reference"/>
                                             </xsl:apply-templates>
                                             
                                             <xsl:apply-templates select="." mode="running.foot.mode">
                                                            <xsl:with-param name="master-reference" select="$titlepage-master-reference"/>
                                             </xsl:apply-templates>
                                             
                                             <fo:flow flow-name="xsl-region-body">
                                                            <xsl:call-template name="set.flow.properties">
                                                                           <xsl:with-param name="element" select="local-name(.)"/>
                                                                           <xsl:with-param name="master-reference"
                                                                                          select="$titlepage-master-reference"/>
                                                            </xsl:call-template>
                                                            
                                                            <fo:block id="{$id}">
                                                                           <xsl:call-template name="part.titlepage"/>
                                                            </fo:block>
                                                            <xsl:copy-of select="$additional.content"/>
                                             </fo:flow>
                              </fo:page-sequence>
               </xsl:template>
               
               <xsl:template match="part/docinfo|partinfo"></xsl:template>
               <xsl:template match="part/title"></xsl:template>
               <xsl:template match="part/titleabbrev"></xsl:template>
               <xsl:template match="part/subtitle"></xsl:template>
               
                
<!-- this modifies the object ids on output so that a double xinclude, such as the 
     bundled set of all books for a component, have unique ids
  -->
<xsl:template name="object.id">
  <xsl:param name="object" select="."/>

  <xsl:variable name="id" select="@id"/>
  <xsl:variable name="xid" select="@xml:id"/>

  <xsl:variable name="preceding.id"
        select="count(preceding::*[@id = $id])"/>

  <xsl:variable name="preceding.xid"
        select="count(preceding::*[@xml:id = $xid])"/>

  <xsl:choose>
    <xsl:when test="$object/@id and $preceding.id != 0">
      <xsl:value-of select="concat($object/@id, $preceding.id)"/>
    </xsl:when>
    <xsl:when test="$object/@id">
      <xsl:value-of select="$object/@id"/>
    </xsl:when>
    <xsl:when test="$object/@xml:id and $preceding.xid != 0">
      <xsl:value-of select="concat($object/@id, $preceding.xid)"/>
    </xsl:when>
    <xsl:when test="$object/@xml:id">
      <xsl:value-of select="$object/@xml:id"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="generate-id($object)"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="*[@role = 'fo-only']" priority='10'>
    <xsl:apply-imports/>
</xsl:template>
</xsl:stylesheet>
