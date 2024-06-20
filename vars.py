import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import warnings
from matplotlib.patches import Patch

# Mod dictionaries
QPSK = {
    '0' : [-1, -1], # 00
    '1' : [-1, 1], # 01
    '2' : [1, -1], # 10
    '3' : [1, 1] # 11
}

QAM16 = {
    "0" : [-3, -3], # 0000
    "1" : [-3, -1], # 0001
    "2" : [-3, 3], # 0010
    "3" : [-3, 1], # 0011
    "4" : [-1, -3], # 0100
    "5" : [-1, -1], # 0101
    "6" : [-1, 3], # 0110
    "7" : [-1, 1], # 0111
    "8" : [3, -3], # 1000
    "9" : [3, -1], # 1001
    "10" : [3, 3], # 1010
    "11" : [3, 1], # 1011
    "12" : [1, -3], # 1100
    "13" : [1, -1], # 1101
    "14" : [1, 3], # 1110
    "15" : [1, 1] # 1111
}

# Color Arrays
COLOR = {
    "0" : 'black', 
    "1" : 'red', 
    "2" : 'green', 
    "3" : 'blue', 
    "4" : 'cyan', 
    "5" : 'magenta', 
    "6" : 'yellow', 
    "7" : 'orange', 
    "8" : 'purple', 
    "9" : 'brown', 
    "10" : 'pink', 
    "11" : 'gray', 
    "12" : 'olive', 
    "13" : 'lime', 
    "14" : 'teal', 
    "15" : 'navy'
}

# Mod lists
MOD4 = list(QPSK.values())
MOD16 = list(QAM16.values())

# Legend Elements
LEG4 = [Patch(facecolor = COLOR["0"], edgecolor = COLOR["0"], label = '00'),
                   Patch(facecolor = COLOR["1"], edgecolor = COLOR["1"], label = '01'),
                   Patch(facecolor = COLOR["2"], edgecolor = COLOR["2"], label = '10'),
                   Patch(facecolor = COLOR["3"], edgecolor = COLOR["3"], label = '11')]

LEG16 = [Patch(facecolor = COLOR["0"], edgecolor = COLOR["0"], label = '0000'),
                     Patch(facecolor = COLOR["1"], edgecolor = COLOR["1"], label = '0001'),
                     Patch(facecolor = COLOR["2"], edgecolor = COLOR["2"], label = '0010'),
                     Patch(facecolor = COLOR["3"], edgecolor = COLOR["3"], label = '0011'),
                     Patch(facecolor = COLOR["4"], edgecolor = COLOR["4"], label = '0100'),
                     Patch(facecolor = COLOR["5"], edgecolor = COLOR["5"], label = '0101'),
                     Patch(facecolor = COLOR["6"], edgecolor = COLOR["6"], label = '0110'),
                     Patch(facecolor = COLOR["7"], edgecolor = COLOR["7"], label = '0111'),
                     Patch(facecolor = COLOR["8"], edgecolor = COLOR["8"], label = '1000'),
                     Patch(facecolor = COLOR["9"], edgecolor = COLOR["9"], label = '1001'),
                     Patch(facecolor = COLOR["10"], edgecolor = COLOR["10"], label = '1010'),
                     Patch(facecolor = COLOR["11"], edgecolor = COLOR["11"], label = '1011'),
                     Patch(facecolor = COLOR["12"], edgecolor = COLOR["12"], label = '1100'),
                     Patch(facecolor = COLOR["13"], edgecolor = COLOR["13"], label = '1101'),
                     Patch(facecolor = COLOR["14"], edgecolor = COLOR["14"], label = '1110'),
                     Patch(facecolor = COLOR["15"], edgecolor = COLOR["15"], label = '1111')]