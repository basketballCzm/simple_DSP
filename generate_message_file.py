#!/usr/bin/env python
# -*- coding: utf-8 -*-
# by caster 

import os
import sys
import time
from lxml import etree
import struct
import ctypes
import ipaddress

file_name="/var/www/data/advertisement.data"
message_buffer=(
# TrafficAnalysis
[
        "\x14",     #version
        "\x09",     #msg_type
        "$1X1",  #msg_id
        "231f",  #time
        "1\xef\xf1""1",  #msg_len
        "ab14",          #PolicyID
        "\x01",          #Packet Type 
        "\x07",          #Packet Subtype 
        "\x8A\xDD\xF5\x53",          #R_StartTime 
        "\x8A\xFD\xF5\x53",          #R_EndTime
        "\x01",          #R_Freq
        "\x12",          #R_Temp_Name_Len
        "XDRFiledsTempt2046",        #R_Temp_Name
        "\x01",          #R_Interface
        "\x04",          
        "\xC0\xA8\x00\x04",          #R_DestIP
        "user_nsn",          #UserName 
        "pwd__nsn",          #Password 
        "\x04",             
        "\xC0\xA8\x00\x04",          #SDTP_IP
],
# Mirror Traffic
[
        "\x14"     ,#version
        "\x05"     ,#msg_type
        "f20\x04"  ,#msg_id
        "231f"  ,#time
        "1\xef\xf1""1"  ,#msg_len
        "ad4Z"          ,#PolicyID
        "\x01"          ,#MProtocalType
        "\x02"          ,#MAppType
        "\x01\x00"          ,#MAppID
        "\x03"          ,#MDstIpSegmentNum
        "\x04"          ,#   
        "\x4f\x10\x10\x01"  ,# ip 
        "\x04"          ,#MDstIpPrefixLen
        "\x04"          ,#   
        "\x4f\x1a\x10\x01"  ,# ip 
        "\x2f"          ,#MDstIpPrefixLen
        "\x10"          ,#   
        "\x4f\x10""123456789abc""\x10\x01"  ,# ipv6 
        "\x0a"          ,#MDstIpPrefixLen
        "\x02"          ,#MSrcPortNum
        "\x15\x00"          ,#MSrcPort
        "\x16\x00"          ,#MSrcPort
        "\x02"               ,#MDstPortNum
        "\x17\x00"
        "\x18\x00"          ,#MDstPort
        "\x04\x00"              ,#MDataMatchOffset
        "\x0e"     
        "match_word1234"    ,#MDataMatchContent
        "\x8A\x9D\xF5\x53"  ,#M_StartTime 
        "\xAA\xAD\xF5\x53"  ,#M_EndTime
        "\x00"              ,#M_GroupNo
        "\x03"              ,#M_Direction
        "\x00"              ,#MFlowAdd
        "\x04"              ,#MType
],
#Bind User
[
        "1"     ,#version
        "\x02"     ,#msg_type
        "3i23"  ,#msg_id
        "3123"  ,#time
        "12\xf1""1"  ,#msg_len
        "0"     ,#bind_action
        "\x0a"
        "user56789a",#username
        "\2"         ,#multi_number
        "\4"         ,#ip_length
        "\x4fzxc"  ,#ip 
        "\x0f"     ,#ip_prefix
        "\4"         ,#ip_length
        "\x4e\x7fZc",#ip
        "\x16"     ,#ip_prefix
],
#block url
[
        "2"     ,#version
        "\x07"     ,#msg_type
        "3124"  ,#msg_id
        "231a"  ,#time
        "1\xef\xf1""1"  ,#msg_len
        "9527"          ,#PolicyID
        "\x01"     ,#action
        "\x01"      ,#block_type
        "\x02"      ,#blocknum
        "\x04\x00"      ,#length , LITTLE_ENDIAN
        "1\xf1""ac" ,#ipv4
        "\x2e\x00"      ,#length , LITTLE_ENDIAN
        "http://junstapo.baijia.baidu.com/article/25996"
],
#block ip
[
        "2"     ,#version
        "\x07"     ,#msg_type
        "3124"  ,#msg_id
        "231a"  ,#time
        "1\xef\xf1""1"  ,#msg_len
        "9527"          ,#PolicyID
        "\x01"     ,#action
        "\x02"      ,#block_type
        "\x02"      ,#blocknum
        "\x04\x00"      ,#length , LITTLE_ENDIAN
        "1\xf1""af" ,#ipv4
        "\x10\x00"      ,#length , LITTLE_ENDIAN
        "12345678E012\xff""456" ,#ipv6
],
#ResourceLib
[
        "\x14"     ,#version
        "\x04"     ,#msg_type
        "3120"  ,#msg_id
        "231f"  ,#time
        "1\xef\xf1""1"  ,#msg_len
        "\x01"          ,#Resoure_Type
        "\x02"          ,#Protocol_Version
        "\x04"
        "\xC0\xA8\x01\x04"  ,# ip 192.168.1.4 
        "\x15\x00"          ,# port
        "user_nsn"          ,# username
        "pwd__nsn"          ,# password
        "\x0f\x00"
        "IPAT_01_000.csv"           ,#filename
],
#TrafficManage
[
        "\x14"     ,#version
        "\x06"     ,#msg_type
        "3120"  ,#msg_id
        "231f"  ,#time
        "1\xef\xf1""1"  ,#msg_len
        "ab14"          ,#PolicyID
        "\x01"          ,#ProtocolType,useless in this message
        "\x01"          ,#AppType
        "\x04"          ,#AppID
        "\x00"
        ""              ,#no AppName
        "\x0a\x00\x00\x00"  ,#AppTraffic_Up_ABS akbps
        "\x0a\x00\x00\x00"  ,#AppTraffic_Down_ABS akbps
        "\x0a\x00\x00\x00"  ,#AppSession_Up_ABS 10 sessions/s
        "\x0a\x00\x00\x00"  ,#AppSession_Down_ABS 10 sessions/s
        "\x0a"                ,#QoS_Label_Up
        "\x0b"                ,#QoS_Label_Down
        "\x02"
        "\x8A\x9D\xF5\x53"          ,#Starttime 
        "\x8A\xAD\xF5\x53"          ,#Stoptime 
        "\x8A\xBD\xF5\x53"          ,#Starttime 
        "\x8A\xCD\xF5\x53"          ,#Stoptime  
],
#SiteVisit
[
        "\x14"     ,#version
        "\x08"     ,#msg_type
        "3120"  ,#msg_id
        "231f"  ,#time
        "1\xef\xf1""1"  ,#msg_len
        "abOP"          ,#PolicyID
        "\x01"          ,#Action
        "\x11"          ,#SRC_AREAGROUP_ID
        "\x04"          ,#SRC_Area_Num
        "\x0d"
        "IPv4_Example1"              ,#SRC_Area_Name
        "\x0d"
        "IPv4_Example2"              ,#SRC_Area_Name
        "\x0d"
        "IPv4_Example3"              ,#SRC_Area_Name
        "\x0d"
        "IPv4_Example4"              ,#SRC_Area_Name
        "\x14"          ,#DEST_AREAGROUP_ID
        "\x02"          ,#DEST_Area_Num
        "\x0d"
        "IPv4_Example5"              ,#DEST_Area_Name
        "\x0d"
        "IPv4_Example6"              ,#DEST_Area_Name
],
)

message_buffer_tmp=([
    "\x14",     #version
    "\x09",     #msg_type
    "$1X1",  #msg_id
    "231f",  #time
    "1\xef\xf1""1",  #msg_len
    "ab14",          #PolicyID
    "\x01",          #Packet Type 
    "\x07",          #Packet Subtype 
    "\x8A\xDD\xF5\x53",          #R_StartTime 
    "\x8A\xFD\xF5\x53",          #R_EndTime
    "\x01",          #R_Freq
    "\x12",          #R_Temp_Name_Len
    "XDRFiledsTempt2046",        #R_Temp_Name
    "\x01",          #R_Interface
    "\x04",          
    "\xC0\xA8\x00\x04",          #R_DestIP
    "user_nsn",          #UserName 
    "pwd__nsn",          #Password 
    "\x04",             
    "\xC0\xA8\x00\x04",          #SDTP_IP
],
)

def save_message_file(message_list):
    if type(message_list)==list:
        s_message=''.join (message_list)
    else:
        s_message=message_list
    #print s_message+"\n"
    while os.path.isfile(file_name) :
        print "old message file existing ,waiting for program to remove it after handling"
        time.sleep(3)
    f_handler=open ( file_name, 'w+b' ) 
    f_handler.write(s_message);
    f_handler.close
    os.system("cp "+file_name+" "+file_name+"."+str(int(time.time()))+" &")
    print "save and backup message.data . done."
    return s_message

def parse_xml(xml_file_name):
    content = open(xml_file_name, 'rb').read()
    parser  = etree.XMLParser(remove_blank_text=True)
    root    = etree.XML(content,parser)

    handle_node(root);

def handle_node(node):
    if type(node) != etree._Element :
        print type(node)
        return ""
    if not node.text :
        node.text=''
    s_type=node.get('type')
    if s_type and globals().has_key("handle_node_"+s_type.lower()) :
        print "handle node "+s_type.lower()
        ret = ( globals()["handle_node_"+s_type.lower()] )(node)
    elif node.tag in ['root','section','body','multi','include']:
        print "handle node "+node.tag
        ret = ( globals()["handle_node_"+node.tag] )(node)
    else:
        print "unknow node :%s ,type :%s"%(node.tag,s_type)
        ret = handle_node_nop(node)

    callbacks=node.get("callback") 
    if callbacks:
        for callback in callbacks.split() :
           if callback and globals().has_key(callback):
               ret = ( globals()[callback] )(ret)
    return ret

def handle_node_nop(node):
    return traversal_child(node)

def handle_node_include(node):
    if not node.get("src"):
        return ""
    else:
        try:
            content = open(node.get("src"), 'rb').read()
            parser  = etree.XMLParser(remove_blank_text=True)
            root    = etree.XML(content,parser)
            return handle_node(root);
        except Exception,e :
            print "error in handle_node_include(), e = ",e
            return ""

def handle_node_section(node):
    return traversal_child(node)

def handle_node_byte(node):
    if not node.text:
        return '\x00'
    value=int(node.text)
    if value<0 or value >255:
        print "error handle_node_byte() wrong number out of range"
        return '\x00'
    else:
        return str(bytearray([value,]))

def handle_node_short(node):
    if not node.text:
        return '\x00'*2
    value=int(node.text)
    data=ctypes.create_string_buffer(2)
    if value < -32768 or value > 32767 :
        print "error handle_node_short() wrong number out of range"
        return '\x00'*2
    else:
        struct.pack_into("<h", data, 0, value) 
        return ''.join(list(data)) 

def handle_node_int(node):
    if not node.text:
        return '\x00'*4
    s=int_to_chars(int(node.text))
    if not s : 
        print "error handle_node_int() wrong number out of range"
        return '\x00'*4
    else:
        return s 

def handle_node_uint(node):
    if not node.text:
        return '\x00'*4
    value=long(node.text)
    data=ctypes.create_string_buffer(4)
    if value < 0 or value > 4294967295  :
        print "error handle_node_uint() wrong number out of range"
        return '\x00'*4
    else:
        struct.pack_into("<I", data, 0, value) 
        return ''.join(list(data)) 

def handle_node_string(node):
    length=node.get("length")
    if length :
        length=int(length)
        str_len=len(node.text)
        if length < str_len :
            print "error, handle_node_string() string '%s' is too long !"%node.text
            return node.text[:length]
        else:
            return node.text+'\x00'*(length-str_len)
    else:
        counter=node.get('counter')
        length=len(node.text)
        if counter=='short':
            data=ctypes.create_string_buffer(2)
            if length <= 0 or length > 65535:
                print "error, handle_node_string() wrong string length"
                return '\x00'*2
            else:
                struct.pack_into("<H", data, 0, length) 
                return ''.join(list(data)) + node.text
        else:
            data=ctypes.create_string_buffer(1)
            if length <= 0 or length > 255:
                print "error, handle_node_string() wrong string length"
                return '\x00'
            else:
                struct.pack_into("<B", data, 0, length) 
                return ''.join(list(data)) + node.text
            

def handle_node_multi(node):
    first_tag=None
    number=0
    for child in node:
        if ( not first_tag ) and type(child) == etree._Element:
            first_tag = child.tag    
            number +=1
        elif child.tag == first_tag:
            number +=1
            
    counter=node.get('counter')
    if counter=='short':
        data=ctypes.create_string_buffer(2)
        if number <= 0 or number > 65535:
            if number !=0:
                print "error, handle_node_multi() counter: short, wrong multi length"
            return '\x00\x00'
        else:
            struct.pack_into("<H", data, 0, number) 
            ret = ''.join(list(data)) 
    elif counter=='int':
        data=ctypes.create_string_buffer(4)
        if number <= 0 or number > 4294967295:
            if number !=0:
                print "error, handle_node_multi() counter: int, wrong multi length"
            return '\x00\x00\x00\x00'
        else:
            struct.pack_into("<I", data, 0, number) 
            ret = ''.join(list(data)) 
    else:
        data=ctypes.create_string_buffer(1)
        if number <= 0 or number > 255:
            if number !=0:
                print "error, handle_node_multi() counter: byte, wrong multi length"
            return '\x00'
        else:
            struct.pack_into("<B", data, 0, number) 
            ret = ''.join(list(data))
    return ret + traversal_child(node)

def handle_node_ip(node):
    s_ip=ipaddress.ip_address(node.text).packed
    counter=node.get('counter')
    if counter=='short':
        if len(s_ip)==4:
            return '\x04\x00'+s_ip
        else:
            return '\x10\x00'+s_ip
    else:
        if len(s_ip)==4:
            return '\x04'+s_ip
        else:
            return '\x10'+s_ip
            
def handle_node_time(node):
    if node.text == '' or node.text == '0':
        return '\x00'*4
    elif node.text == 'now':
        mk_time=long(time.time())
    else:
        mk_time=long(time.mktime(time.strptime(node.text,'%Y/%m/%d %H:%M:%S')))
    
    data=ctypes.create_string_buffer(4)
    if mk_time < 0 or mk_time > 2**32:
        print "error, handle_node_time() wrong time value "
        return '\x00\x00\x00\x00'
    else:
        struct.pack_into("<I", data, 0, mk_time) 
        ret = ''.join(list(data)) 
        return ret
        

def handle_node_ipv4(node):
    return handle_node_ip(node)

def handle_node_ipv6(node):
    return handle_node_ip(node)

def traversal_child(node):
    ret=""
    for child in node :
        ret=ret+handle_node(child)
    return ret
    
def handle_node_body(node):
    return traversal_child(node)

def handle_node_root(node):
    return traversal_child(node)

def int_to_chars(value):
    print "value=",value
    data=ctypes.create_string_buffer(4)
    if value < -2**31 or value > 2**31-1:
        print "error int_to_chars() wrong number out of range"
        return None 
    else:
        struct.pack_into("<i", data, 0, value)
        print "data="+str(list(data))
        return ''.join(list(data)) 

def fill_message_length(data):
    message_length=int_to_chars(len(data))
    #print "message_length=",message_length
    if message_length:
        return message_length+data
    else:
        return '\x00\x00\x00\x00'+data


if __name__=="__main__":
    if len(sys.argv) < 2:
        print "auto test"
        for message_list in message_buffer:
            save_message_file(message_list)
    else:
        if len(sys.argv) >= 3:
            filename = sys.argv[2]
        parse_xml(sys.argv[1])

    time.sleep(0.1)
    print "all done."
