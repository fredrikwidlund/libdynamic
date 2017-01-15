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
  geom_smooth(method = "loess", span = 0.1, data = data.map, aes(x = size, y = insert, colour = name)) +
  scale_y_continuous(labels = comma) +
  expand_limits(y = 0) +
  scale_x_continuous(trans = log_trans(), breaks = 10^(0:10), labels = comma)
g1$labels$colour <- "Implementation"
ggsave(g1, file = "vector.pdf", width = 10, height = 5)

