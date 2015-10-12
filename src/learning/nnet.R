#!/usr/bin/env r

source('lib/randWeights.R')
source('lib/gradientDescent.R')
source('lib/predict.R')

library(docopt)

'Usage: nnet.R [-i <input> -o <output> -t <train_size> -h <hidden> -a <alpha> -e <epochs>]

options:
  -i --input          <input>        Input file
  -o --output         <output>       Output file
  -t --train-size     <train_size>   Training set size
  -h --hidden-units   <hidden>       Number of hidden units
  -a --learning-rate  <alpha>        Learning rate (alpha)
  -e --iteration      <epochs>       Number of iterations (epochs)' -> doc

opts <- docopt(doc)

data_file   <- opts$input
out_file    <- opts$output
train_size  <- as.integer(eval(parse(text = opts$train_size)))
hidden      <- as.integer(eval(parse(text = opts$hidden)))
alpha       <- as.integer(eval(parse(text = opts$alpha)))
epochs      <- as.integer(eval(parse(text = opts$epochs)))

set.seed(400)
options(warn=-1)

data_set <- read.csv(data_file, header=F)

# create training set
size <- 1:nrow(data_set)
train_index <- sample(size, train_size)
train_set <- as.matrix(data_set[train_index,-5])
train_labels <- as.matrix(data_set[train_index,5])

# test set consists of all examples not in the training set
test_set <- as.matrix(data_set[-train_index,-5])
test_labels <- as.matrix(data_set[-train_index,5])

# determine architecture of neural net from the dimensions of the data
In <- ncol(train_set)
Out <- 1
Architecture <- c(In, hidden, Out)

Theta1 <- randWeights(In,hidden, 0.12)
Theta2 <- randWeights(hidden,Out, 0.12)
Theta <- as.matrix(c(Theta1,Theta2))

results <- gradientDescent(train_set, train_labels, Architecture, Theta, alpha, epochs)

Theta <- as.matrix(results[[1]])
J_hist <- as.matrix(results[[2]])

write.table(Theta, out_file, row.names=F, col.names=F, sep=",")

#prediction(Architecture, Theta, t(as.matrix(train_set[1,])))

png(filename="img/j.png")
plot(J_hist,type="l",xlab="Epoch",ylab="Error",ylim=c(0,max(J_hist)),
    panel.first=c(abline(v=(seq(-1000,epochs,epochs/10)),col="gray",
    lty=1),abline(h=(seq(-1000,max(J_hist),0.1)),col="gray",lty=1)))
#title(main=paste("hidden units:",hidden,"\n","alpha:",alpha,"\n","Training
    #examples:",train_size, sep=" "))
dev.off()

