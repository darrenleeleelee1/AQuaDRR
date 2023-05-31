#!/usr/bin/env python3
import argparse
import re
import os
import math
import subprocess
from datetime import datetime

def circle(x, y, radius, num_points=100):
    angles = [i * 2 * math.pi / num_points for i in range(num_points)]
    points = [(x + radius * math.cos(a), y + radius * math.sin(a)) for a in angles]
    return points

def convert_to_gdt(input_file, output_file):
    with open(input_file, 'r') as f:
        lines = f.readlines()

    gdt_output = []
    cell_count = 0
    idx = 0
    po = 0.2 # pin_offset
    wo = 0.3 # wire_offset
    width = int(lines[0].split()[1]) # Extract chip width from the first line
    height = int(lines[1].split()[1]) # Extract chip height from the second line
    total_WL = lines[3].split()[1] # Extract total WL from the fourth line
    cost = lines[4].split()[1] # Extract cost from the fifth line

    current_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    while idx < len(lines):
        line = lines[idx].strip()
        idx += 1

        if line.startswith("Net_id"):
            net_id = int(line.split()[1])
            gdt_output.append(f"cell {{ c={current_time} m={current_time} 'Net{net_id}'\n")

        elif line.startswith("pin_num"):
            pin_num = int(line.split()[1])
            for i in range(pin_num):
                pin_line = lines[idx].strip()
                idx += 1
                pin_coords = re.findall(r'\d+', pin_line)
                x, y, z = pin_coords
                x = float(x); y = float(y)
                box_center_x, box_center_y = ((x+po + x+1-po)/2), ((y+po + y+1-po)/2)
                gdt_output.append(f"t{{ {255} tt255 xy({box_center_x} {box_center_y}) '{net_id}'}}\n")
                gdt_output.append(f"b{{ {z} dt0 xy({x+po} {y+po} {x+po} {y+1-po} {x+1-po} {y+1-po} {x+1-po} {y+po})}}\n")

        elif line.startswith("Via_num"):
            via_num = int(line.split()[1])
            for i in range(via_num):
                via_line = lines[idx].strip()
                idx += 1
                via_coords = re.findall(r'\d+', via_line)
                x, y, z1, z2 = via_coords
                x = float(x); y = float(y);
                z1 = int(z1); z2 = int(z2);
                low_points = circle(x + 0.5, y + 0.5, 0.3); low_point_str = " ".join(f"{px} {py}" for px, py in low_points);
                high_points = circle(x + 0.5, y + 0.5, 0.4); high_point_str = " ".join(f"{px} {py}" for px, py in high_points)
                gdt_output.append(f"b{{ {z1} dt2 xy({low_point_str})}}\n")
                gdt_output.append(f"b{{ {z2} dt2 xy({high_point_str})}}\n")

        elif line.startswith("H_segment_num"):
            h_segment_num = int(line.split()[1])
            for i in range(h_segment_num):
                h_line = lines[idx].strip()
                idx += 1
                h_coords = re.findall(r'\d+', h_line)
                x1, y1, z1, x2, y2, z2 = h_coords
                x1 = float(x1); y1 = float(y1);
                x2 = float(x2); y2 = float(y2);         
                gdt_output.append(f"b{{ {z1} dt1 xy({x1+wo} {y1+wo} {x1+wo} {y2-wo} {x2-wo} {y2-wo} {x2-wo} {y1+wo})}}\n")

        elif line.startswith("V_segment_num"):
            v_segment_num = int(line.split()[1])
            for i in range(v_segment_num):
                v_line = lines[idx].strip()
                idx += 1
                v_coords = re.findall(r'\d+', v_line)
                x1, y1, z1, x2, y2, z2 = v_coords
                x1 = float(x1); y1 = float(y1); 
                x2 = float(x2); y2 = float(y2);
                gdt_output.append(f"b{{ {z1} dt1 xy({x1+wo} {y1+wo} {x1+wo} {y2-wo} {x2-wo} {y2-wo} {x2-wo} {y1+wo})}}\n")

            # finish a cell
            gdt_output.append("}\n")
            cell_count += 1
        


    with open(output_file, 'w') as f:
        f.write("gds2{1\n")
        f.write(f"m={current_time} a={current_time}\n") # Updated header with valid date
        f.write("lib 'mylib' 0.001 1e-09\n")
        f.write(f"cell{{ c={current_time} m={current_time} 'All'\n")
        # Add Chip bounding box
        # f.write(f"b{{ 255 dt0 xy(0 0 0 {height} {width} {height} {width} 0)}}\n")
        # Add bounding box with lines
        f.write(f"path{{ 255 dt0 xy(0 0 {width} 0)}}\n")  # Bottom line
        f.write(f"path{{ 255 dt0 xy(0 0 0 {height})}}\n")  # Left line
        f.write(f"path{{ 255 dt0 xy(0 {height} {width} {height})}}\n")  # Top line
        f.write(f"path{{ 255 dt0 xy({width} 0 {width} {height})}}\n")  # Right line
        # Add total WL and cost above bounding box
        f.write(f"t{{ 255 tt255 xy({width/2} {height+2}) 'File: {input_file}'}}\n") 
        f.write(f"t{{ 255 tt255 xy({width/2} {height+4}) 'Total WL: {total_WL}'}}\n")
        f.write(f"t{{ 255 tt255 xy({width/2} {height+6}) 'Cost: {cost}'}}\n")
        for net_id in range(cell_count):
            f.write(f"s{{ 'Net{net_id}' xy(0 0)}}\n")
        f.write("}\n")
        for line in gdt_output:
            f.write(line)
        f.write("}\n")

def convert_to_gds(input_file, output_file):
    # implement this function to convert gdt to gds
    command = ["./gdt2gds", input_file, output_file]
    subprocess.run(command, check=True)

def main():
    parser = argparse.ArgumentParser(description='Converts a routing result to GDT format.')
    parser.add_argument('-t', '--txt_to_gdt', nargs=2, metavar=('input_txt', 'output_gdt'),
                        help='Converts a routing result TXT file to GDT format.')
    parser.add_argument('-s', '--gdt_to_gds', nargs=2, metavar=('input_gdt', 'output_gds'),
                        help='Converts a GDT file to GDS format.')
    parser.add_argument('-ts', '--txt_to_gds', nargs=2, metavar=('input_txt', 'output_gds'),
                        help='Converts a routing result TXT file to GDT format and then to GDS format.')
    args = parser.parse_args()

    if args.txt_to_gdt:
        convert_to_gdt(args.txt_to_gdt[0], args.txt_to_gdt[1])
    if args.gdt_to_gds:
        convert_to_gds(args.gdt_to_gds[0], args.gdt_to_gds[1])
    if args.txt_to_gds:
        temp_gdt_file = os.path.splitext(args.txt_to_gds[1])[0] + ".gdt"
        convert_to_gdt(args.txt_to_gds[0], temp_gdt_file)
        convert_to_gds(temp_gdt_file, args.txt_to_gds[1])
        os.remove(temp_gdt_file)  # remove the temporary gdt file

if __name__ == "__main__":
    main()

"""
GDS Format Description

* [Layer]/[Type]
Type:
[Layer]/0: Pins
[Layer]/1: Wires
[Layer]/2: Vias

* 255/[Type]
255/0: Chip bounding box
255/255: All text
"""