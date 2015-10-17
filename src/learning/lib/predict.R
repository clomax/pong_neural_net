source('lib/sigmoid.R')
source('lib/splitTheta.R')

prediction <- function(Arch, Theta, X)
{

    # Get Theta1/2 back
    t <- splitTheta(Arch, Theta)
    Theta1 <- as.matrix(t[[1]])
    Theta2 <- as.matrix(t[[2]])

    a1 <- t(as.matrix(c(1,X)))
    a2 <- sigmoid(a1 %*% Theta1)

    a2 <- as.matrix(cbind(1,a2))
    a3 <- sigmoid(a2 %*% Theta2)

    return(a3)
}
