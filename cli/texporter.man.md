---
title: TEXPORTER
section: 1
header: General Commands Manual
footer: texpainter
date: 2021-03-02
---

# NAME
texporter - Exports texpainter documents

# SYNOPSIS
texporter **--help**

texporter [*options*] input output

# DESCRIPTION

Texporter is the command line interface for rendering texpainter(1) documents. The tool is intended
for batch processing, for example, to render muliple texpainter documents, or as part of a build
automation process.

# OPTIONS

**--supersampling=n**
: Sets the supersampling level. n may vary from 1 to 8, inclusive

# FILES

texporter does not use any configuration files

# ENVIRONMENT

texporter does not use any environment variables

# BUGS

See GitHub Issues: [https://github.com/milasudril/texpainter/issues](https://github.com/milasudril/texpainter/issues)

# AUTHOR

Torbjörn Rathsman