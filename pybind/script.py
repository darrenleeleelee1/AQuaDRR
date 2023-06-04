import time
import sys
import router
import quadtree

def main(input_file, output_file):
    start = time.time()

    print("Reading Layout")
    read_start = time.time()
    layout = router.Layout()
    router.readLayout(layout, input_file)
    print(f"Read time: {time.time() - read_start}")

    print("Routing")
    routing_start = time.time()
    r = router.Router(layout)
    r.main()
    print(f"Reroute number: {r.num_of_reroute}")
    r.countNumOfViolation()
    print(f"Violation number: {r.num_of_violation}")
    print(f"Routing time: {time.time() - routing_start}")

    print("Writing Layout")
    write_start = time.time()
    router.writeLayout(layout, output_file)
    print(f"Write time: {time.time() - write_start}")

    print(f"Total time: {time.time() - start}")
    
    print("QuadTree Processing")
    quadtree_start = time.time()
    qtree = quadtree.QTree(5, layout.width, layout.height) 
    quadtree_addpoint_start = time.time()
    # Access the grid's cur_paths
    for i in range(len(r.grid.graph)):
        for j in range(len(r.grid.graph[i])):
            cur_paths = r.grid.graph[i][j][0].cur_paths
            if len(cur_paths) >= 2:
                qtree.add_point(i, j)
    print(f"QuadTree add_point time: {time.time() - quadtree_addpoint_start}")
    quadtree_subdivide_start = time.time()
    qtree.subdivide()
    print(f"QuadTree subdivide time: {time.time() - quadtree_subdivide_start}")
    quadtree_graph_start = time.time()
    qtree.quadtree_graph()
    print(f"QuadTree graph time: {time.time() - quadtree_graph_start}")
    print(f"QuadTree time: {time.time() - quadtree_start}")
            

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 script.py <input_file> <output_file>")
        sys.exit(1)
        
    main(sys.argv[1], sys.argv[2])
