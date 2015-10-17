cost <- function(h,y)
{
    m <- nrow(h)
    J <- ((1/(2*m)) * sum((h-y)^2))
    return(J)
}
