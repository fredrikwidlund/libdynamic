#!/usr/bin/env Rscript

library(ggplot2)
library(scales)
library(sitools)
library(gridExtra)

data.map <- read.csv("vector.dat", head=TRUE, sep=",")

g1 <- ggplot(legend = TRUE) +
  labs(list(title = "Insert", x = "Vector size", y = "Time/Operation (ns)")) +
  theme(plot.title = element_text(size = 10),
        axis.title.x = element_text(size = 8), axis.title.y = element_text(size = 8),
        axis.text.x = element_text(size = 8), axis.text.y = element_text(size = 8)) +
  geom_smooth(method = "loess", span = 0.2, se = FALSE, data = data.map, aes(x = size, y = insert, colour = name)) +
  scale_y_continuous(trans = log_trans(), breaks = c(1,2,3,4,5,6,7,8,9,10,20,30,40,50,60,70,80,90,100,200,300,400,500), labels = comma) +
  scale_x_continuous(trans = log_trans(), breaks = 10^(0:10), labels = comma)
g1$labels$colour <- "Library"
pdf("vector.pdf", width = 10, height = 5)
grid.arrange(g1, ncol=1, top = "libdynamic vector benchmark")
