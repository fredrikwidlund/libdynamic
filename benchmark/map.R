#!/usr/bin/env Rscript

library(ggplot2)
library(scales)
library(sitools)

data.dynamic_map <- read.csv("dynamic_map.dat", head=TRUE, sep=",")
data.unordered_map<- read.csv("unordered_map.dat", head=TRUE, sep=",")

graph <- ggplot(legend = TRUE) + 
  ggtitle('ns/operation') +
  theme(plot.title = element_text(size = 10), 
        axis.title.x = element_text(size = 8), axis.title.y = element_text(size = 8),
        axis.text.x = element_text(size = 8), axis.text.y = element_text(size = 8)) + 
  geom_line(data = data.dynamic_map, aes(x = size, y = insert, colour = "libdynamic(insert)")) +
  geom_line(data = data.dynamic_map, aes(x = size, y = at, colour = "libdynamic(at)")) +
  geom_line(data = data.unordered_map, aes(x = size, y = insert, colour = "std::unordered_map(insert)")) +
  geom_line(data = data.unordered_map, aes(x = size, y = at, colour = "std::unordered_map(at)")) +
  scale_y_continuous(labels = comma) +
  expand_limits(y = 0) +
  scale_x_continuous(trans = log_trans(), breaks = 10^(2:10), labels = comma) +
  scale_colour_manual("", values = c("#E69F00", "#56B4E9", "#D55E00", "#009E73", "#0072B2"))
ggsave(graph, file = "map.pdf", width = 10, height = 5)
