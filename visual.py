#!/usr/bin/python3

import glob
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
from matplotlib.patches import Rectangle
import argparse


parser = argparse.ArgumentParser()
parser.add_argument('-i', type=str, help='directory of the input test path')
parser.add_argument('-o', type=str, help='directory of the output draw path')
args = parser.parse_args()

in_path = args.i
file_list = glob.glob(f"./{in_path}/*.txt")
out_path = args.o

for count, file in enumerate(file_list):
    file_name = file.split('/')[-1][:-4]
    with open(file, 'r') as f:

        #read width
        line = f.readline().split()
        width = int(line[1])

        #read height
        line = f.readline().split()
        height = int(line[1])

        #read layer
        line = f.readline().split()
        layer = int(line[1])

        #read total WL
        line = f.readline().split()
        wl = int(line[1])

        #read total cost
        line = f.readline().split()
        cost = int(line[1])

        #read obstacles
        line = f.readline().split()
        obs_num = int(line[1])
        h_obs_list = []
        v_obs_list = []
        for i in range(obs_num):
            line = f.readline().split()
            assert(int(line[2]) == int(line[5]))
            if int(line[2]) == 0:
                h_obs_list.append([int(line[0]), int(line[1]), int(line[3]), int(line[4])])
            else:
                v_obs_list.append([int(line[0]), int(line[1]), int(line[3]), int(line[4])])

        pin_list = []
        pin_id_list = []
        via_list = []
        h_wire_list = []
        v_wire_list = []
        #read nets
        line = f.readline().split()
        net_num = int(line[1])
        for i in range(net_num):
            #read net id
            line = f.readline().split()
            net_id = int(line[1])
            
            #read pins
            line = f.readline().split()
            pin_num = int(line[1])
            for _ in range(pin_num):
                line = f.readline().split()
                pin_list.append([int(line[0]), int(line[1]), int(line[2])])
                pin_id_list.append(net_id)

            #read vias
            line = f.readline().split()
            via_num = int(line[1])
            for _ in range(via_num):
                line = f.readline().split()
                via_list.append([int(line[0]), int(line[1])])

            #read horizontal wires
            line = f.readline().split()
            h_seg_num = int(line[1])
            for _ in range(h_seg_num):
                line = f.readline().split()
                h_wire_list.append([int(line[0]), int(line[1]), int(line[3]), int(line[4]), int(line[2])])

            #read vertical wires
            line = f.readline().split()
            v_seg_num = int(line[1])
            for _ in range(v_seg_num):
                line = f.readline().split()
                v_wire_list.append([int(line[0]), int(line[1]), int(line[3]), int(line[4]), int(line[2])])
        
        fig = plt.figure()
        ax = fig.add_subplot(111)
        ax.set_aspect('equal',adjustable='box')
        for obs in h_obs_list:
            rect = Rectangle((obs[0], obs[1]), obs[2]-obs[0], obs[3]-obs[1], color='darkgreen',fill=False, hatch='xx', alpha = 1)
            ax.add_patch(rect)

        for obs in v_obs_list:
            rect = Rectangle((obs[0], obs[1]), obs[2]-obs[0], obs[3]-obs[1], color='navy',fill=False, hatch='xx', alpha = 1)
            ax.add_patch(rect)
        
        # # Create lighter shades of blue and green
        # complementary_green = mcolors.to_rgba('#FF1E00', alpha=0.3)  # Complementary color to green
        # complementary_blue = mcolors.to_rgba('#FFA900', alpha=0.5)  # Complementary color to blue

        for wire in h_wire_list:
            if(wire[4] == 0):
                rect = Rectangle((0.25 + wire[0], 0.25 + wire[1]),wire[2]-wire[0]-0.5, 0.5, color="green", alpha = 0.3)
            else:
                rect = Rectangle((0.25 + wire[0], 0.25 + wire[1]),wire[2]-wire[0]-0.5, 0.5, color="red", alpha = 0.3)
            ax.add_patch(rect)

        for wire in v_wire_list:
            if(wire[4] == 1):
                rect = Rectangle((0.25 + wire[0], 0.25 + wire[1]),0.5, wire[3]-wire[1]-0.5, color="blue", alpha = 0.5)
            else:
                rect = Rectangle((0.25 + wire[0], 0.25 + wire[1]),0.5, wire[3]-wire[1]-0.5, color="black", alpha = 0.5)
            ax.add_patch(rect)

        for pin, id in zip(pin_list, pin_id_list):
            rect = Rectangle((pin[0],pin[1]), 1, 1, color="orange", alpha = 0.5)
            ax.add_patch(rect)
            ax.annotate(id, (pin[0] + 0.5, pin[1] + 0.5), color='black', fontsize=3, ha='center', va='center')

        for via in via_list:
            rect = Rectangle((0.25 + via[0], 0.25 + via[1]), 0.5, 0.5, color="red", fill=False,alpha = 1)
            ax.add_patch(rect)
        plt.xlim([0,width])
        plt.ylim([0,height])
        plt.title(f"{file_name} WL = {wl}, cost = {cost}")
        plt.savefig(f"./{out_path}/{file_name}.png", dpi=300)
        plt.close(fig)
        print(f"Success create ./{out_path}/{file_name}.png")