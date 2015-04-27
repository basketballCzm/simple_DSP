var parseData=require('./parse_data.js');
var filename='user_map.data';
var mmappath='./build/Release/mmap';

console.log(parseData.handler(filename,mmappath));
