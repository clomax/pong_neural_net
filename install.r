#!/usr/bin/r

if(is.null(argv) | length(argv)<1)
{
  q()
}

repos <- "http://cran.rstudio.com"
lib.loc <- "/usr/local/lib/R/site-library"
install.packages(argv, lib.loc, repos)
