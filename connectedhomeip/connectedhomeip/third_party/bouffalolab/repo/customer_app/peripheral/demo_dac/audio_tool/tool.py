    
import os
import sys
import numpy as np
import math

if __name__ == "__main__":
    data_list_deal = []
    path =  sys.argv[1]
    path_save = sys.argv[2]
    bytes_num = os.path.getsize(path)

    save_file = open(path_save, "wb")

    with open(path, 'rb') as f:
        for i in range(bytes_num // 2):
            data = f.read(2)
            data_int = int.from_bytes(data, byteorder='little', signed=True)
            data_int = int((data_int +32768 ) * 0.0156099794003204)
            new_data = data_int.to_bytes(2, byteorder='little', signed=False)
            save_file.write(new_data)
            save_file.write(new_data)
    save_file.close()

