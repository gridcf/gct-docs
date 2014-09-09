<?xml version="1.0"  encoding="iso-8859-1" ?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">

                <!-- default width of tables -->
                <xsl:param name="default.table.width" select="'700px'"/>

                <!-- do static inclusion of header, instead of php include -->
                <xsl:param name="static.includes"/>
                
                <!-- speed up the chunking process? -->
                <xsl:param name="chunk.fast">1</xsl:param>
                
                <!-- which css stylesheet to use?
                <xsl:param name="html.stylesheet" select="'/toolkit/css/default.css'"></xsl:param> -->
                
                <!-- update target database with all possible cross references (for olinks)-->
                <xsl:param name="collect.xref.targets">yes</xsl:param>
                
                <!-- default location for target database document (for olinks) -->
                <xsl:param name="target.database.document">olinkdb.xml</xsl:param> 
                
                <!-- Set path to admonition graphics  -->
                <xsl:param name="admon.graphics.path"><xsl:value-of select="$topdir"/>/docbook/xsl/docbook-xsl-1.78.1/images/</xsl:param>
                <!-- What is the filename extension for admon graphics? -->
                <xsl:param name="admon.graphics.extension" select="'.png'"/>
                
                <!-- Set path to callout graphics -->
                <xsl:param name="callout.graphics.path"><xsl:value-of select="$topdir"/>/docbook-images/callouts/</xsl:param>
                
                <!-- Depth to which sections should be chunked -->
                <xsl:param name="chunk.section.depth">0</xsl:param>
                
                <!-- Are parts automatically enumerated? -->
                <xsl:param name="part.autolabel">0</xsl:param>
                
                <!-- how deep should each toc be? (how many levels?) -->
                <xsl:param name="toc.max.depth">4</xsl:param>
                
                <!-- How deep should recursive sections appear in the TOC for chapters? -->
                 <xsl:param name="toc.section.depth">4</xsl:param>
                
                <!-- Should the first section be chunked separately from its parent? > 0 = yes-->
                <xsl:param name="chunk.first.sections">1</xsl:param>
                
                <!-- Instead of using default filenames, use ids for filenames (dbhtml directives take precedence) -->
                <xsl:param name="use.id.as.filename">1</xsl:param>
                
                <!-- custom toc - book only shows chapter 
                <xsl:template match="preface|chapter|reference|appendix|article" mode="toc">
                                <xsl:param name="toc-context" select="."/>
                                
                                <xsl:choose>
                                                <xsl:when test="local-name($toc-context) = 'book'">
                                                                <xsl:call-template name="subtoc">
                                                                                <xsl:with-param name="toc-context" select="$toc-context"/>
                                                                                <xsl:with-param name="nodes" select="foo"/>
                                                                </xsl:call-template>
                                                </xsl:when>
                                                <xsl:otherwise>
                                                                <xsl:call-template name="subtoc">
                                                                                <xsl:with-param name="toc-context" select="$toc-context"/>
                                                                                <xsl:with-param name="nodes"
                                                                                                select="section|sect1|glossary|bibliography|index
                                                                                                |bridgehead[$bridgehead.in.toc != 0]"/>
                                                                </xsl:call-template>
                                                </xsl:otherwise>
                                </xsl:choose>
                </xsl:template>
                -->
                <!-- control TOCs -->
                <xsl:param name="generate.toc">
                                appendix  toc,title
                                article/appendix  nop
                                article   toc,title
                                book      toc,title
                                chapter   toc,title
                                part      toc,title
                                preface   toc,title
                                qandadiv  toc
                                qandaset  toc
                                reference toc,title
                                sect1     toc
                                sect2     toc
                                sect3     toc
                                sect4     toc
                                sect5     toc
                                section   toc
                                set       toc,title
                </xsl:param>
                

                <!-- INDEX PARAMETERS -->
               
                <!-- Select indexterms based on type attribute value -->
                <xsl:param name="index.on.type">1</xsl:param>
                
                <!-- GLOSSARY PARAMETERS -->
                
                <!-- permit wrapping of long lines of code
                <xsl:attribute-set name="monospace.verbatim.properties" 
                                use-attribute-sets="verbatim.properties monospace.properties">
                                <xsl:attribute name="wrap-option">wrap</xsl:attribute>
                </xsl:attribute-set> -->
                
                <!-- INCORPORATING DOCBOOK PAGES INTO WEBSITE -->

                <!-- make sure there's a DOCTYPE in the html output (otherwise, some css renders strangely -->
                <xsl:param name="chunker.output.doctype-public" select="'-//W3C//DTD HTML 4.01 Transitional//EN'"/>
                <xsl:param name="chunker.output.doctype-system" select="'http://www.w3.org/TR/html4/loose.dtd'"/>
                <!-- add elements to the HEAD tag -->
                <!-- the following template is for the conditional comments for detecting certain browsers -->
                <xsl:template name="conditionalComment">
                                <xsl:param name="qualifier" select="'IE 7'"/>
                                <xsl:param name="contentRTF" select="''" />
                                <xsl:comment>[if <xsl:value-of select="$qualifier"/>]<![CDATA[>]]>
                                                <xsl:copy-of select="$contentRTF" />
                                                <![CDATA[<![endif]]]></xsl:comment>
                </xsl:template>
                
                <xsl:template name="user.head.content">
                                <link href="http://www.globus.org/toolkit/css/default.css" rel="stylesheet" type="text/css" /> 
                                <link rel="stylesheet" type="text/css" href="/toolkit/css/print.css" media="print" />

                                <xsl:comment> calling in special style sheet if detected browser is IE 7 </xsl:comment>

                                <xsl:call-template name="conditionalComment">
                                                <xsl:with-param name="qualifier" select="'IE 7'"/>
                                                <xsl:with-param name="contentRTF">
                                                                &lt;link rel="stylesheet" type="text/css" href="/toolkit/css/ie7.css" /&gt;
                                                </xsl:with-param>
                                </xsl:call-template>                                

                                <link rel="alternate" title="Globus Toolkit RSS" href="/toolkit/rss/downloadNews/downloadNews.xml" type="application/rss+xml"/>
                                <script>
                                                <xsl:comment>
                                                function GlobusSubmit()
                                                {
                                                var f=document.GlobusSearchForm;
                                                
                                                f.action="http://www.google.com/custom";
                                                if (f.elements[0].checked) {
                                                f.q.value = f.qinit.value + " -inurl:mail_archive " ;
                                                } else {
                                                f.q.value = f.qinit.value + " inurl:mail_archive " ;
                                                }
                                                }
                                                </xsl:comment>
                                </script>
                </xsl:template>
                
                <!-- add an attribute to the BODY tag -->
                <xsl:template name="body.attributes">
                                <xsl:attribute name="class">section-3</xsl:attribute>
                </xsl:template>
                
                <!-- pull in 'website' with this code by modifying chunk-element-content from html/chunk-common.xsl-->
                <xsl:template name="chunk-element-content">
                                <xsl:param name="prev"/>
                                <xsl:param name="next"/>
                                <xsl:param name="nav.context"/>
                                <xsl:param name="content">
                                                <xsl:apply-imports/>
                                </xsl:param>
                                
                                <xsl:call-template name="user.preroot"/>
                                
                                <html>
                                                <xsl:call-template name="html.head">
                                                                <xsl:with-param name="prev" select="$prev"/>
                                                                <xsl:with-param name="next" select="$next"/>
                                                </xsl:call-template>
                                                
                                                <body>
                                                                <xsl:call-template name="body.attributes"/>
                                                                
                                                                
 
                                                                <xsl:call-template name="user.header.navigation"/>
                                                                
                                                                <xsl:call-template name="header.navigation">
                                                                                <xsl:with-param name="prev" select="$prev"/>
                                                                                <xsl:with-param name="next" select="$next"/>
                                                                                <xsl:with-param name="nav.context" select="$nav.context"/>
                                                                </xsl:call-template>
                                                                
                                                                <xsl:call-template name="user.header.content"/>
                                                                
                                                                <xsl:if test="$static.includes = 1">
                                                                    <xsl:copy-of select="document('includes/docbook_sidebar.inc')"/>
                                                                </xsl:if>
                                                                <xsl:if test="$static.includes != 1">
                                                                    <xsl:processing-instruction name="php">
                                                                        <xsl:text>include_once("</xsl:text><xsl:value-of select="$topdir"/><xsl:text>/includes/docbook_sidebar.inc");?</xsl:text>
                                                                                    </xsl:processing-instruction>
                                                                </xsl:if>
                                                                <xsl:if test="$draft.mode = 'yes'">
                                                               
                                                                <!-- add temporary DRAFTS box here until docs are released  -->
                                                               <xsl:processing-instruction name="php">
            <xsl:text>include_once("</xsl:text>
            <xsl:value-of select="$topdir"/>
            <xsl:text>/includes/docbook_drafts.inc");</xsl:text>
            <xsl:text>?</xsl:text></xsl:processing-instruction>
            </xsl:if>
                                                               

                                                


                                                                
                                                                <xsl:copy-of select="$content"/>
                                                                
                                                                <xsl:call-template name="user.footer.content"/>
                                                                
                                                                <xsl:call-template name="footer.navigation">
                                                                                <xsl:with-param name="prev" select="$prev"/>
                                                                                <xsl:with-param name="next" select="$next"/>
                                                                                <xsl:with-param name="nav.context" select="$nav.context"/>
                                                                </xsl:call-template>
                                                                
                                                                <xsl:call-template name="user.footer.navigation"/>

                                                      
                                                </body>
                                </html>
                </xsl:template>
                
                <!-- prevent h1 and h2 using clear: both - want to control in css, instead -->
                <xsl:template name="section.heading">
                                <xsl:param name="section" select="."/>
                                <xsl:param name="level" select="'1'"/>
                                <xsl:param name="title"/>
                                <xsl:element name="h{$level+1}">
                                                <xsl:attribute name="class">title</xsl:attribute>
                                                <a>
                                                                <xsl:attribute name="name">
                                                                                <xsl:call-template name="object.id">
                                                                                                <xsl:with-param name="object" select="$section"/>
                                                                                </xsl:call-template>
                                                                </xsl:attribute>
                                                </a>
                                                <xsl:copy-of select="$title"/>
                                </xsl:element>
                </xsl:template>
                
                <!-- taking out top table row of Navigational Header -->
                
                <xsl:template name="header.navigation">
                                <xsl:param name="prev" select="/foo"/>
                                <xsl:param name="next" select="/foo"/>
                                <xsl:param name="nav.context"/>
                                
                                <xsl:variable name="home" select="/*[1]"/>
                                <xsl:variable name="up" select="parent::*"/>
                                
                                <xsl:variable name="row1" select="$navig.showtitles != 0"/>
                                <xsl:variable name="row2" select="count($prev) &gt; 0
                                                or (count($up) &gt; 0
                                                and generate-id($up) != generate-id($home)
                                                and $navig.showtitles != 0)
                                                or count($next) &gt; 0"/>
                                
                                <xsl:if test="$suppress.navigation = '0' and $suppress.header.navigation = '0'
                                                ">
                                                <div class="navheader">
                                                                <xsl:if test="$row1 or $row2">
                                                                                <table width="100%" summary="Navigation header">
                                                                                                
                                                                                                <xsl:if test="$row1">
                                                                                                                <!-- 
                                                                                                                <tr>
                                                                                                                <th colspan="3" align="center">
                                                                                                                <xsl:apply-templates select="." mode="object.title.markup"/>
                                                                                                                </th>
                                                                                                                </tr>
                                                                                                                -->
                                                                                                </xsl:if>
                                                                                                
                                                                                                <xsl:if test="$row2">
                                                                                                                <tr>
                                                                                                                                <td width="20%" align="left">
                                                                                                                                                <xsl:if test="count($prev)>0">
                                                                                                                                                                <a accesskey="p">
                                                                                                                                                                                <xsl:attribute name="href">
                                                                                                                                                                                                <xsl:call-template name="href.target">
                                                                                                                                                                                                                <xsl:with-param name="object" select="$prev"/>
                                                                                                                                                                                                </xsl:call-template>
                                                                                                                                                                                </xsl:attribute>
                                                                                                                                                                                <xsl:call-template name="navig.content">
                                                                                                                                                                                                <xsl:with-param name="direction" select="'prev'"/>
                                                                                                                                                                                </xsl:call-template>
                                                                                                                                                                </a>
                                                                                                                                                </xsl:if>
                                                                                                                                                <xsl:text>&#160;</xsl:text>
                                                                                                                                </td>
                                                                                                                                <th width="60%" align="center">
                                                                                                                                                <xsl:choose>
                                                                                                                                                                <xsl:when test="count($up) > 0
                                                                                                                                                                                and generate-id($up) != generate-id($home)
                                                                                                                                                                                and $navig.showtitles != 0">
                                                                                                                                                                                <xsl:apply-templates select="$up" mode="object.title.markup"
                                                                                                                                                                                                />
                                                                                                                                                                </xsl:when>
                                                                                                                                                                <xsl:otherwise>&#160;</xsl:otherwise>
                                                                                                                                                </xsl:choose>
                                                                                                                                </th>
                                                                                                                                <td width="20%" align="right">
                                                                                                                                                <xsl:text>&#160;</xsl:text>
                                                                                                                                                <xsl:if test="count($next)>0">
                                                                                                                                                                <a accesskey="n">
                                                                                                                                                                                <xsl:attribute name="href">
                                                                                                                                                                                                <xsl:call-template name="href.target">
                                                                                                                                                                                                                <xsl:with-param name="object" select="$next"/>
                                                                                                                                                                                                </xsl:call-template>
                                                                                                                                                                                </xsl:attribute>
                                                                                                                                                                                <xsl:call-template name="navig.content">
                                                                                                                                                                                                <xsl:with-param name="direction" select="'next'"/>
                                                                                                                                                                                </xsl:call-template>
                                                                                                                                                                </a>
                                                                                                                                                </xsl:if>
                                                                                                                                </td>
                                                                                                                </tr>
                                                                                                </xsl:if>
                                                                                </table>
                                                                </xsl:if>
                                                                <xsl:if test="$header.rule != 0">
                                                                                <hr/>
                                                                </xsl:if>
                                                </div>
                                </xsl:if>
                </xsl:template>
                
                <xsl:template match="*[@role = 'html-only']" priority='10'>
                    <xsl:apply-imports select="."/>
                </xsl:template>
                

</xsl:stylesheet>
