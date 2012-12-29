<?xml version="1.0" encoding="ISO-8859-1"?>

<html xsl:version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <style> <!-- style sheet for document -->
      h1 {background-color: #d0d0d0} <!-- add a background to make headings stand out -->
      h2 {background-color: #d0d0d0}
      .highlight {font-weight:bold; color:blue}  <!-- highlighted text style -->
      .normal {font-weight:normal}               <!-- normal text style      -->
    </style> 
  </head>
  <body>
    <h1>UH-Ferret: Document comparison</h1>

    <!-- display top-level information -->
    <p>Common trigrams: <xsl:value-of select="uhferret/common-trigrams"/></p>
    <p>Similarity: <xsl:value-of select="uhferret/similarity"/></p>

    <!-- work through each document -->
    <xsl:for-each select="uhferret/document">
      <!-- display document-level information -->
      <h2>Document: <xsl:value-of select="source"/></h2>
      <p>Number of trigrams: <xsl:value-of select="num-trigrams"/></p>
      <p>Containment in other document: <xsl:value-of select="containment"/></p>

      <!-- work through each block in text, displaying as highlighted or normal -->
      <pre>
      <xsl:for-each select="text/block">
        <xsl:if test="@text='copied'">
          <span class="highlight"><xsl:value-of select="."/></span>
        </xsl:if>
        <xsl:if test="@text='normal'">
          <span style="normal"><xsl:value-of select="."/></span>
        </xsl:if>
      </xsl:for-each>
      </pre>
    </xsl:for-each>
  </body>
</html>

