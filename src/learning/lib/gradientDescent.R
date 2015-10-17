source('lib/cost.R')
source('lib/train.R')

gradientDescent <- function(X,y,Arch,Theta,alpha,epochs) {

    m <- nrow(X)
    J_hist <- matrix(0,epochs,1)

    t <- splitTheta(Arch, Theta)
    Theta1 <- as.matrix(t[[1]])
    Theta2 <- as.matrix(t[[2]])

    for (i in 1:epochs) {
        BP <- train(Arch,Theta,X,y)
        h <- BP[[1]]
        grad <- as.matrix(BP[[2]])

        J_hist[i] <- cost(h,y)
        Theta <- Theta - (alpha * 1/m) * grad
        cat(sprintf("Epoch %s of %s -- %s\n", i, epochs, J_hist[i]))
    }

    return(list(Theta, J_hist))
}
