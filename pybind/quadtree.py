import matplotlib.pyplot as plt
import matplotlib.patches as patches
from PIL import Image
import numpy as np

class Node:
    def __init__(self, x0, y0, w, h, points, parent=None):
        self.x0 = x0
        self.y0 = y0
        self.width = w
        self.height = h
        self.points = points
        self.children = []
        self.parent = parent

    def get_width(self):
        return self.width

    def get_height(self):
        return self.height

    def get_points(self):
        return self.points

    def get_parent(self):
        return self.parent


class QTree:
    def __init__(self, k, chip_size_w=5, chip_size_h=5):
        self.threshold = k
        self.points = []
        self.root = Node(0, 0, chip_size_w, chip_size_h, self.points)

    def add_point(self, x, y):
        self.points.append((x, y))

    def subdivide(self):
        recursive_subdivide(self.root, self.threshold)

    def find_point(self, x, y):
        return self._find_point(self.root, x, y)

    def _find_point(self, node, x, y):
        if contains_point(node, x, y):
            if node.children:
                for child in node.children:
                    result = self._find_point(child, x, y)
                    if result is not None:
                        return result
            return node
        else:
            return None
    def get_node_at_level(self, x, y, level):
        """Find the node containing the point and return the node `level` levels up the tree."""
        node = self.find_point(x, y)
        for _ in range(level):
            if node.get_parent() is not None:
                node = node.get_parent()
        return node
    
    def print_path_to_root(self, x, y):
        """Find the node containing the point and print its path to the root."""
        node = self.find_point(x, y)
        while node is not None:
            print("Quadrant location: (%f, %f), size: (%f, %f)" % (node.x0, node.y0, node.get_width(), node.get_height()))
            node = node.get_parent()


    def quadtree_graph(self):
        fig, ax = plt.subplots(figsize=(8, 8))
        plt.title("Quadtree")
        c = find_children(self.root)
        
        for n in c:
            ax.add_patch(patches.Rectangle((n.x0, n.y0), n.width, n.height, fill=False))
        x = [i[0] for i in self.points]
        y = [i[1] for i in self.points]
        plt.plot(x, y, 'ro', markersize=4)
        ax.set_xlim(-1, self.root.get_width() + 1)
        ax.set_ylim(-1, self.root.get_height() + 1)
        plt.savefig('quadtree.png')
    
    def highlight_graph(self, highlight_node=None):
        fig, ax = plt.subplots(figsize=(8, 8))
        plt.title("Quadtree")
        c = find_children(self.root)

        # Draw black rectangles
        for n in c:
            if n is not highlight_node:
                ax.add_patch(patches.Rectangle((n.x0, n.y0), n.width, n.height, fill=False, edgecolor='black'))

        # Draw the red rectangle
        if highlight_node is not None:
            ax.add_patch(patches.Rectangle((highlight_node.x0, highlight_node.y0), highlight_node.width, highlight_node.height, fill=False, edgecolor='red'))
            
        x = [i[0] for i in self.points]
        y = [i[1] for i in self.points]
        colors = ['red' if highlight_node and contains_point(highlight_node, *pt) else 'grey' for pt in self.points]
        plt.scatter(x, y, color=colors, s=8)
        ax.set_xlim(-1, self.root.get_width() + 1)
        ax.set_ylim(-1, self.root.get_height() + 1)
        plt.savefig('highlight_quadtree.png')

def recursive_subdivide(node, k):
    if len(node.points)<=k:
        return
    w_ = float(node.width/2)
    h_ = float(node.height/2)
    p = contains(node.x0, node.y0, w_, h_, node.points)
    x1 = Node(node.x0, node.y0, w_, h_, p, parent=node)
    recursive_subdivide(x1, k)

    p = contains(node.x0, node.y0 + h_, w_, h_, node.points)
    x2 = Node(node.x0, node.y0 + h_, w_, h_, p, parent=node)
    recursive_subdivide(x2, k)

    p = contains(node.x0 + w_, node.y0, w_, h_, node.points)
    x3 = Node(node.x0 + w_, node.y0, w_, h_, p, parent=node)
    recursive_subdivide(x3, k)

    p = contains(node.x0 + w_, node.y0 + h_, w_, h_, node.points)
    x4 = Node(node.x0 + w_, node.y0 + h_, w_, h_, p, parent=node)
    recursive_subdivide(x4, k)

    node.children = [x1, x2, x3, x4]


def contains(x, y, w, h, points):
    pts = []
    for point in points:
        if x <= point[0] < x+w and y <= point[1] < y+h:
            pts.append(point)
    return pts


def contains_point(node, x, y):
    return node.x0 <= x < node.x0 + node.width and node.y0 <= y < node.y0 + node.height


def find_children(node):
    if not node.children:
        return [node]
    else:
        children = []
        for child in node.children:
            children += find_children(child)
        return children


if __name__ == "__main__":
    qtree = QTree(2, 50, 50)
    # qtree.add_point(1, 2); qtree.add_point(2, 3); qtree.add_point(3, 4)
    # qtree.add_point(2, 2)  ; qtree.add_point(3, 3); qtree.add_point(4, 4)
    qtree.subdivide()
    qtree.quadtree_graph()

    point = (3, 4)
    qtree.print_path_to_root(*point)
    
    node = qtree.get_node_at_level(*point, 2)
    print("Quadrant location: (%f, %f), size: (%f, %f)" % (node.x0, node.y0, node.get_width(), node.get_height()))
    qtree.highlight_graph(highlight_node=node)