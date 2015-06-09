/*
 * tair - test/client.test.js
 * Copyright(c) 2012 Taobao.com
 * Author: kate.sf <kate.sf@taobao.com>
 */

var cli = require('tair');
var should = require('should');
var sys = require('sys')
var exec = require('child_process').exec

var tair;
var nm=3;
describe('client.test.js', function () {

  before(function (done) {
    tair = new cli('group_1', [
      {host: '192.168.2.201', port: 5198}
    ], {heartBeatInterval: 3000},
      function (err) {
        if (err) {
          done(err);
          return;
        }
        done();
      });
  });
  
  it("insert ad.group test data should be ok!",function(done){
      done()
    })

  it("c++ test ,ad_request should be ok!",function(done){
      exec(__dirname+'/ad_map_test',function(err,stdout,stderr){
          sys.print('stdout:'+ stdout)
          sys.print('stderr:'+ stderr)
          if(err !== null)
          {
            console.log('exec error: '+err)
          }
          done()
        })
    })

  it("clear up test data should work",function(done){
      var key_list=['xiang','zadd.test.key','zadd.test.key2','sadd.int.test.key','alargeData',];
      var count=0
      for(var i=0;i<key_list.length;i++){
        console.log('remove '+key_list[i])
        tair.remove(key_list[i],nm,function(err){
          should.not.exist(err)
          count++;
          if(count==key_list.length){
             done();
          }
        })
      }
    })

});
