import time
import router
import sys

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

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 script.py <input_file> <output_file>")
        sys.exit(1)
        
    main(sys.argv[1], sys.argv[2])
