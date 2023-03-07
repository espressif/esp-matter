import argparse
import shutil
import os

def main():
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument('--outputdir', type=str, required=True, help='input the path of output folder')
    arg_parser.add_argument('--bl_path', type=str, required=True, help='input the path of bootloader')
    arg_parser.add_argument('--bt_path', type=str, required=True, help='input the path of bluetooth driver binary')
    arg_parser.add_argument('--wifi_path', type=str, required=True, help='input the path of wifi driver binary')
    args = arg_parser.parse_args()
    
    bl_new_path = os.path.join(args.outputdir, os.path.basename(args.bl_path))
    bt_new_path = os.path.join(args.outputdir, os.path.basename(args.bt_path))
    wifi_new_path = os.path.join(args.outputdir, os.path.basename(args.wifi_path))
    
    if os.path.exists(bl_new_path):
        os.remove(bl_new_path)
    shutil.copyfile(args.bl_path, os.path.join(args.outputdir, bl_new_path))
    
    if os.path.exists(bt_new_path):
        os.remove(bt_new_path)   
    shutil.copyfile(args.bt_path, os.path.join(args.outputdir, bt_new_path))
    
    if os.path.exists(wifi_new_path):
        os.remove(wifi_new_path)   
    shutil.copyfile(args.wifi_path, os.path.join(args.outputdir, wifi_new_path))

if __name__ == "__main__":
    main()
