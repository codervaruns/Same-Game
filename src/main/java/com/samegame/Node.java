package com.samegame;

import java.util.ArrayList;
import java.util.List;

// Node structure for graph representation
public class Node {
    public int row;
    public int col;
    public char color;
    public boolean active;
    public List<Integer> neighbors;  // indices of adjacent nodes
    
    public Node(int r, int c, char clr) {
        this.row = r;
        this.col = c;
        this.color = clr;
        this.active = true;
        this.neighbors = new ArrayList<>();
    }
}
