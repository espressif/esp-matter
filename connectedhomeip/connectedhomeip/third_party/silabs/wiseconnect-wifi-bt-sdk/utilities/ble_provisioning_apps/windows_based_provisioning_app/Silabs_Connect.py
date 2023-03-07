import tkinter,os,asyncio,binascii
from tkinter import *
from tkinter import messagebox,simpledialog
from bleak import BleakScanner,BleakClient
from PIL import Image, ImageTk


client,conn="",""
x,wlandevices,MAC_Address,bledevices=list(),list(),list(),list()
uuid1="00001aa1-0000-1000-8000-00805f9b34fb"
uuid2="00001bb1-0000-1000-8000-00805f9b34fb"
uuid3="00001cc1-0000-1000-8000-00805f9b34fb"
version,framewlan,frameble,ble_rescan,fwversion="","","","",""
ipaddr,macaddr,ssid="","",""
wlan_disconnected=[0,0]
lbl_ip=""

        
main=Tk()
main.title("	Silabs Connect",)
main.geometry("900x700")
main.config(bg='white')

main.call('wm', 'iconphoto', main._w, PhotoImage(file='silabs_icon.png'))

def fscan():
    async def run():
        scanner=BleakScanner()
        global bledevices
        await scanner.start()
        await asyncio.sleep(2.0)
        bledevices.clear()
        bledevices=await scanner.get_discovered_devices()
        
    loop=asyncio.get_event_loop()
    loop.run_until_complete(run())
    #print("ble_devices:",bledevices)
    count=0
    global frameble
    check=isinstance(frameble,str)
    if not check:
        frameble.destroy()
    frameble=Frame(main,width=50,height=60,bd=1)

    for d in bledevices:
        if str(d).startswith(("00:23:a7","00:23:A7","88:DA:1A","80:C9:55","88:da:1a","80:c9:55")):
            count+=1
    if count==0:
        fscan()
    else:
        r=0
        for d in bledevices:
            d=str(d)
            if d.startswith(("00:23:a7","00:23:A7","88:DA:1A","80:C9:55","88:da:1a","80:c9:55")):
                bledev=d[18:]+"\n"+d[:17]
                btn_bledev=Button(frameble,text=bledev,bd=1,height=2,width=50,command=lambda addr=d:connble(addr),bg="#0597AA",fg="white",font=("Verdana 10 bold",10))
                #print(address)
                btn_bledev.grid(row=r,column=0,columnspan=3,pady=5)
                r+=1
        frameble.grid(row=3,column=0,columnspan=3)
        #dev.grid(row=2,column=0,columnspan=3)
        statusmsg.set('Select a BLE device')

def connble(addr):
    statusmsg.set('Connecting to BLE Device')
    address=addr[:17]
    
    async def run():
        global client
        global conn
        print("came here")
        client=BleakClient(address)
        try:
            await client.connect()
            statusmsg.set('BLE Device Connected')
            conn=True
        except Exception as e:
            statusmsg.set('Some exception raised... Try again')
            conn=False
            messagebox.showinfo("status","BLE Connection Failed, Try Again")
            print(e) 
    loop=asyncio.get_event_loop()
    loop.run_until_complete(run())
    print(conn)
    if conn:
        afterble(addr[18:],addr[:17])


statusmsg=StringVar()
status=Label(main,textvariable=statusmsg)
status.place(relx=0.0,rely=1.0,anchor='sw')
statusmsg.set('Scan for RS911X Devices...')

title=Label(main,text="RS911x BLE Provisioning",bd=2,height=2,width=47,fg="#0597AA",font=("Verdana 10 bold",25))
title.grid(row=0,column=0,columnspan=3,pady=5)

image1 = Image.open("bluetooth_logo.png")
test = ImageTk.PhotoImage(image1)
logo= tkinter.Label(image=test,bd=0)
logo.place(x=1, y=4)

image2 = Image.open("wifi_logo.png")
test1 = ImageTk.PhotoImage(image2)
logo= tkinter.Label(image=test1,bd=0)
logo.place(x=820, y=4)

blescan=Button(main,text="BLE SCAN ON",bd=1,height=2,width=50,bg="#0597AA",fg="white",command=fscan,font=("Verdana 10 bold",10))
blescan.grid(row=1,column=0,columnspan=3,pady=10)

#f1=Frame(main,width=50,height=60,bd=1)

def getfwversion():
    async def run():
        try:
            #global fw_version
            global fwversion
            #Getting the FW version cmdid 8/ASCII 0x38
            #Writing data on 0x1AA1
            await client.write_gatt_char(uuid1, b'\x38', response=True)
            #reading data on 0x1BB1
            fw_version = await client.read_gatt_char(uuid2)
            await asyncio.sleep(0.5)
            fwversion=fw_version[2:].decode()
            #fw_version="".join(map(chr, fw_version))
            print("\tFW version is:  ",fwversion)
        
        except Exception as e:
            print(e)
            
    loop=asyncio.get_event_loop()
    loop.run_until_complete(run())
def notification_handler(sender,data):
    wlandevices.append(data)
def getwlandevices():
    global ssid
    global framewlan
    async def run():
        try:
        #WLAN status if response is '0' WLAN not connected, other than '0' connected
            #cmdid 7/ASCII 0x37
            wlandevices.clear()
            await client.start_notify(uuid3,notification_handler)
            await client.write_gatt_char(uuid1, b'\x37', response=True)
            #reading data on 0x1BB1
            await asyncio.sleep(1)
            WLAN_state = await client.read_gatt_char(uuid2)
            print("\tWLAN state is: {0}".format(WLAN_state))
            WLAN_state=WLAN_state.decode()
            if(WLAN_state[1]=='\x00'):
                            
                #WLAN scan enable
                #cmdid 3/ASCII 0x33
                await client.write_gatt_char(uuid1, b'\x33', response=True)
                #reading data on 0x1BB1, this is optional. it is not required
                await asyncio.sleep(2.0)
                num_of_wlan_scan_results = await client.read_gatt_char(uuid2)
                print("\tNumber of WLAN scan results are: {0}".format(num_of_wlan_scan_results[1]))
                
                #await asyncio.sleep(10.0)
                #await client.stop_notify(uuid3)
                
                print(wlandevices)
                if num_of_wlan_scan_results[1]==0:
                    print("AP's are not found")
                    messagebox.showinfo("scan status", "No APs Found, Retrying For WLAN Scan") 
                    
                    wlan_disconnected[1]=1
                    
                    #sys.exit()
                else:
                    if len(wlandevices)==0:
                            await asyncio.sleep(5.0)
                    x.clear()
                    for mobiles in wlandevices:
                        ap_name=mobiles[2:].decode().rstrip('\x00')
                        x.append(mobiles[0])
                        x.append(ap_name)
                    #made a change here
                    i=1
                    while i < len(x):
                        print(x[i]," ----security type---- ",x[i-1])
                        i=i+2
            else:
                print("Already Connected")
                choice=messagebox.askquestion("status","Already Connected to AP, Do You Want to Disconnect?")
                if choice=='yes':
                    await client.write_gatt_char(uuid1, b'\x34', response=True)
                    await asyncio.sleep(1.0)
                    status = await client.read_gatt_char(uuid2)
                    messagebox.showinfo("status","Wi-Fi DISCONNECTED")
                    wlan_disconnected[0]=1     
                    
                else:
                    f=open("ip_mac.txt","r")
                    J=f.read()
                    J=J.split(" ")
                    f.close()
                    #afterwlan(ipaddr,macaddr,ssid)
                    wlan_disconnected[0]=2
                    Member=' '.join(J[2:])
                    afterwlan(J[0],J[1],Member)
                #sys.exit()
                
        except Exception as e:
            print("Some exception raised")
            print(e)
            sys.exit()
        
    loop=asyncio.get_event_loop()
    loop.run_until_complete(run())
    if wlan_disconnected[0]==1:
        wlan_disconnected[0]=0
        getwlandevices()
    elif wlan_disconnected[1]==1:
        wlan_disconnected[1]=0
        getwlandevices()
        
    elif wlan_disconnected[0]==0:
        framewlan=Frame(main,width=50,height=100,bd=1)   
        framewlan.grid(row=4,column=0,columnspan=3)
        statusmsg.set('Select Desired AP To Connect')
        r,d=0,1
        while d<len(x):
            ssid=x[d]
            if x[d-1]==2:
                ap_sec="WPA2"
            elif x[d-1]==0:
                ap_sec="OPEN"
            else:
                ap_sec=""
            btn_wlandev=Button(framewlan,text=ssid+"\n"+ap_sec,bd=1,height=2,width=30,command=lambda ssid=x[d]:connwlan(ssid),bg="#0597AA",fg="white",font=("Verdana 10 bold",10))
            btn_wlandev.grid(row=r,column=0,columnspan=2,pady=5)
            r+=1
            d+=2
    #elif wlan_disconnected[1]==1:
    #    wlan_disconnected[1]=0
    #    getwlandevices()
    else:
        print("Please check this")
       

def connwlan(ssid):
    print("Came to wlan conn: ",ssid)
    global ipaddr
    global macaddr
    if client.is_connected:
    
        async def run():
            try:
                #WLAN status if response is '0' WLAN not connected, other than '0' connected
                #cmdid 7/ASCII 0x37
                await client.write_gatt_char(uuid1, b'\x37', response=True)
                #reading data on 0x1BB1
                await asyncio.sleep(1.0)
                WLAN_state = await client.read_gatt_char(uuid2)
                print("\tWLAN state is: {0}".format(WLAN_state))
                WLAN_state=WLAN_state.decode()
                if(WLAN_state[1]=='\x00'):
                    name=b'\x32'+b'\x00'+b'\x00'+bytes(ssid,'ascii')
                    #print("Checking security type")
                    await client.write_gatt_char(uuid1, name, response=True)
                    #result=await client.read_gatt_char(uuid2)
                    #print("After sending ssid: ",result)
                    j=1
                    while j <= len(x):
                        if ssid==x[j]:
                            security=x[j-1]
                            print(security)
                            break
                        else:
                            j=j+2
                        
                    #sending security type, cmd id is 5/ASCII 0x35
                    #print("Enter Security type: ")
                    #security=input()
                    sec_type=b'\x35'+b'\x00'+b'\x00'+bytes(str(security),'ascii')
                    print(sec_type)
                    await client.write_gatt_char(uuid1, sec_type, response=True)
                    await asyncio.sleep(2.0)

                    if security==2:
                        #sending WLAN PSK, cmd id is 6/ASCII 0x36
                        #print("Enter PSK: ")
                        #main2.destroy()
                        psk=simpledialog.askstring(title=ssid,prompt="Enter password:",show="*")
                        if not psk is None:
                            password=b'\x36'+b'\x00'+b'\x00'+bytes(psk,'ascii')
                            await client.write_gatt_char(uuid1, password, response=True)
                    else:
                        if security!=0:
                            print("No AP Found")
                            statusmsg.set('AP Not Found, Select Desired AP To Connect')
                            sys.exit()
                          
                    await asyncio.sleep(10.0)
                    #reading data on 0x1BB1
                    WLAN_state = await client.read_gatt_char(uuid2)
                    print("\tWLAN state is: {0}".format(WLAN_state))
                    #WLAN_state=WLAN_state.decode()
                    connect_status=WLAN_state[0]
                    connect=WLAN_state[1]
                    print(connect)
                    print(connect_status)
                    if (connect_status!=2)or(connect!=1):
                        for i in range(1,5):
                            await asyncio.sleep(1.0)
                            #reading data on 0x1BB1
                            WLAN_state = await client.read_gatt_char(uuid2)
                            print("\tWLAN state is: {0}".format(WLAN_state))
                            #WLAN_state=WLAN_state.decode()
                            connect_status=WLAN_state[0]
                            connect=WLAN_state[1]
                            if ((connect_status==2)and(connect==1)):
                                break
                        else:
                            messagebox.showinfo("status","WLAN Connection Failed. Please Try Again")
                        
                    if ((connect_status==2)and(connect==1)):
                        ipaddr=str(WLAN_state[10])+"."+str(WLAN_state[11])+"."+str(WLAN_state[12])+"."+str(WLAN_state[13])
                        MAC_Address.clear()
                        for k in range(3,9):
                            if len(hex(WLAN_state[k])[2:])==1:
                                member=hex(WLAN_state[k])[2:]
                                member1='0'+member
                                MAC_Address.append(member1)
                                continue
                            MAC_Address.append(hex(WLAN_state[k])[2:])
                            macaddr=':'.join(MAC_Address)
                       
                        #macaddr=':'.join(MAC_Address)
                        #messagebox.showinfo("status","Connected to AP successfully")
                        f=open("ip_mac.txt","w")
                        f.write(ipaddr+" "+macaddr+" "+ssid)
                        f.close()
                        framewlan.destroy()
                        afterwlan(ipaddr,macaddr,ssid)
                    else:
                        #messagebox.showinfo("status","WLAN Connection Failed. Please Try Again")
                        print("Connection failed after retries")
                                
                
                else:
                    print("Already connected")
                    choice=messagebox.askquestion(ssid,"WiFi Already connected to AP, Do You Want to Disconnect?")
                    if choice=='yes':
                        #send command to disconnect
                        wlan_disconnect()
                        #getwlandevices()
                    else:
                        framewlan.destroy()
                        f=open("ip_mac.txt","r")
                        J=f.read()
                        J=J.split(" ")
                        f.close()
                        #afterwlan(ipaddr,macaddr,ssid)
                        #print("J[0],J[1],J[2]",J[0],J[1],J[2])
                        Member=' '.join(J[2:])
                        afterwlan(J[0],J[1],Member)
                    #sys.exit()
                    
           
            except Exception as e:
                print("Some excception raised in connwlan : ")
                messagebox.showinfo("status","Check your BLE connection")
                  
             #   getwlandevices()

              #  print(e)
               # sys.exit()
       
        loop=asyncio.get_event_loop()
        loop.run_until_complete(run())
        if not client.is_connected:
            print("BLE got disconnected", client.is_connected)
            Ble_Disconnect() 
    else:
        messagebox.showinfo("status","Check your BLE connection")
        Ble_Disconnect()    
        

 
def afterble(bledev,addr):
    global version
    #global lbl
    global framewlan
    global btn_bleconn
    blescan.destroy()
    #dev.destroy()
    frameble.destroy()
    check=isinstance(ble_rescan,str)
    if not check:
        ble_rescan.destroy()
        
    
    bledev=bledev+"\n"+addr
    print(bledev)
    btn_bleconn=Button(main,text=bledev+"\n"+"BLE Connected",bd=1,height=3,width=30,font=("Verdana 10 bold",10),bg="#555555",fg="white",command=Ble_Disconnect)
    #btn_bleconn.grid(row=1,column=0,columnspan=3,pady=5)
    btn_bleconn.place(x=10, y=100)

    #statusmsg.set('Connected to BLE device.. Select an Access Ponint')
    
    getfwversion()
    print(fwversion)
   # t="Firmware version:   "+fw_version
    
    version=Label(main,text="Firmware Version:"+"\t"+fwversion,bd=1,height=3,relief="solid",width=30,font=("Verdana 10 bold",10),bg="#555555",fg="white")
    #version.grid(row=1,column=0,columnspan=3)
    version.place(x=650, y=100)
    
    #f2=Frame(main,width=75,height=100,bd=1)
    
   # lbl=Label(main,text="Available WLAN devices",bd=1,height=2,width=50,font=(10))
   # lbl.grid(row=3,column=0,columnspan=3)
    
    getwlandevices()

def afterwlan(ipaddr,macaddr,ssid):
    global lbl_mac
    global lbl_ip
    global wlanbtn
    global btn_wlanconn
    global lbl_ip
    blescan.destroy()
    #framewlan.destroy()
    x.clear()
    #btn_wlandev.destroy()
    print(ssid)
    btn_wlanconn=Button(main,text=ssid+"\n"+"Connected",bd=1,height=2,width=40,font=("Verdana 10 bold",10),bg="#0597AA",fg="white",command=wlan_disconnect)
    #wlanbtn=Label(main,text=ssid+"\n"+"Connected",bd=1,height=2,width=30,font=("Verdana 10 bold",10),bg="#0597AA",fg="white",command=None)

    btn_wlanconn.grid(row=2,column=0,columnspan=3,pady=5)
    statusmsg.set('Connected to AP')
    
    lbl_mac=Label(main,text="WLAN MAC Address "+macaddr,bd=1,height=2,width=40,font=("Verdana 10 bold",10),command=None)
    lbl_mac.grid(row=3,column=0,columnspan=3)
    lbl_ip=Label(main,text="IP Address: "+ipaddr,bd=1,height=2,width=40,font=("Verdana 10 bold",10),command=None)
    lbl_ip.grid(row=5,column=0,columnspan=3)
    
def wlan_disconnect():
    btn_wlanconn.destroy()
    lbl_ip.destroy()
    lbl_mac.destroy()
    wlan_disconnected[0]=0
    async def run():
        await client.write_gatt_char(uuid1, b'\x34', response=True)
        await asyncio.sleep(1.0)
        status = await client.read_gatt_char(uuid2)
        messagebox.showinfo("status","WIFI DISCONNECTED")
    
    loop=asyncio.get_event_loop()
    loop.run_until_complete(run())
   
    getwlandevices()
    
def Ble_Disconnect():
    global ble_rescan
    blescan.destroy()

    version.destroy()
    #lbl.destroy()
    x.clear()
    frameble.destroy()
    check=isinstance(framewlan,str)
    if not check:
        framewlan.destroy()
    
    if wlan_disconnected[0]==1:
        framewlan.destroy()
    
    check=isinstance(lbl_ip,str)
    if not check:
        lbl_ip.destroy()
        lbl_mac.destroy()
        btn_wlanconn.destroy()
    
    btn_bleconn.destroy()
    async def run():    
        await client.disconnect()
        print("Connected:",client.is_connected)
        messagebox.showinfo("status","BLE Disconnected")
    
    loop=asyncio.get_event_loop()
    loop.run_until_complete(run())
    
    #if wlan_disconnected[0]==0:
     #   btn_wlandev.destroy()    
    ble_rescan=Button(main,text="BLE SCAN",bd=1,height=2,width=50,bg="#0597AA",fg="white",command=fscan,font=("Verdana 10 bold",10))
    ble_rescan.grid(row=1,column=0,columnspan=3,pady=10) 
    statusmsg.set('Scan for RS911X Devices...')
    
main.mainloop()
