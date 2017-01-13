#!/usr/bin/env Rscript

library(ggplot2)
library(scales)
library(sitools)

data.map <- read.csv("map.dat", head=TRUE, sep=",")
sets <- unique(data.map$name)
graph <- ggplot(legend = TRUE) +
  labs(list(x = "Elements", y = "Time/Operation (ns)")) +
  theme(plot.title = element_text(size = 10), 
        axis.title.x = element_text(size = 8), axis.title.y = element_text(size = 8),
        axis.text.x = element_text(size = 8), axis.text.y = element_text(size = 8)) + 
  geom_smooth(method = "loess", span = 0.5, data = data.map, aes(x = size, y = insert, colour = name)) +
  scale_y_continuous(labels = comma) +
  expand_limits(y = 0) +
  scale_x_continuous(trans = log_trans(), breaks = 10^(2:10), labels = comma)
ggsave(graph, file = "map.pdf", width = 10, height = 5)
