/**
 * 
 * @authors Your Name (you@example.org)
 * @date    2015-04-23 19:19:57
 * @version $Id$
 */
fs = require('fs');
function handler(filename,mmappath){

        var mmap = require(mmappath);
	try{
	var fd = fs.openSync(filename,'rs+');
	}catch(e){	
		console.log("handler:open file error:",e);
	}//catch

        try{
        var stat=fs.statSync(filename);
        }catch(e){
                console.log("handler:stat file error:",e);
        } 

        var buffer = mmap.map(stat.size, mmap.PROT_READ, mmap.MAP_SHARED, fd);
	fs.closeSync(fd);

        var map_size=buffer.readInt32LE(0);
 
        var map_num=buffer.readInt32LE(4);

        var user_pos=new Array(map_num);
        for(i=8,m=0;m<map_num;i=i+20,m++){
            var temp_obj=new Object();

            temp_obj.id=buffer.readInt32LE(i);
            temp_obj.x=buffer.readFloatLE(i+4);
            temp_obj.y=buffer.readFloatLE(i+8);
            temp_obj.z=buffer.readFloatLE(i+12);
            
            user_pos[m]=temp_obj;
        }//for
        
        return JSON.stringify(user_pos);
};//handler

exports.handler=handler;


