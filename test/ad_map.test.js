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
var used_key={}
var mall_id=2

function add_used_key(keys)
{
    if(keys.constructor === Array)
    {
      keys.forEach(function(e){
          used_key[e]=1
        })  
    }
    else
    {
      used_key[keys]=1
    }
}

describe('ad_map.test.js', function () {

  before(function (done) {
    tair = new cli('group_1', [
      {host: 'localhost', port: 5198}
    ], {heartBeatInterval: 3000},
      function (err) {
        if (err) {
          done(err);
          return;
        }
        done();
      });
  });
  
  it("ad_map insert test data location should be ok!",function(done){
      var test_entry_list=[
        ["123456-lt",4.3,3.8,1],
        ["234567-rb",204,100.8,1],
        ["345678-lb",4.3,103.8,1],
        ["abcdef-rt",198.3,2.6,1],
        ["bcdefg-center",100.15,50.3,1],
      ]
      var prop_list=["x","y","z"]
      var count=0
      test_entry_list.forEach(function(c){
          prop_list.forEach(function(d,j){
              var key="location:"+mall_id+":"+c[0]+":"+d
              var value=c[j+1]
              add_used_key(key)
              tair.set(key,value,0,nm,0,function(err,success){
                  should.not.exist(err)
                  success.should.equal(true)
                  count++;
                  if(count==prop_list.length*test_entry_list.length){
                    done()
                  }
                })
            })
        })
    })

  it("ad_map insert test data ad.space should be ok!",function(done){
       var test_entry_list=[
        [23,"space No.23",1,[3,2,4]],
        [28,"space No.28",1,[1,5,6]],
      ]
      var prop_list=["desc","type","ad.group.set"]
      var count=0
      test_entry_list.forEach(function(c){
          prop_list.forEach(function(d,j){
              var key="ad.space:"+mall_id+":"+c[0]+":"+d
              var value=c[j+1]
              add_used_key(key)
              if(d.indexOf("set",d.length-3)===-1)
              {
                tair.set(key,value,0,nm,0,function(err,success){
                    should.not.exist(err)
                    success.should.equal(true)
                    count++;
                    if(count==prop_list.length*test_entry_list.length){
                      done()
                    }
                  })
              }
              else
              {
                var zcount=0
                c[j+1].forEach(function(v){
                    tair.zadd(key,nm,v,v,function(err, data){
                        should.not.exist(err);
                        data.should.equal(true);
                        ++zcount
                        if(zcount==c[j+1].length)
                        {
                          count++ 
                          if(count==prop_list.length*test_entry_list.length){
                            done()
                          }
                        }
                      })
                  })  
              }
            })
        })
      
    })

  it("ad_map insert test data ad.group should be ok!",function(done){
       var test_entry_list=[
        [1,1.0,0.01,23,1,[1,2,7],['label1','label2', 'label3']],
        [2,1.1,0.02,23,1,[3],['label1','label3', 'label6']],
        [3,1.2,0.016,23,1,[4],['label3','label4']],
        [4,1.4,0.014,28,1,[5],['label4','label5']],
        [5,1.5,0.013,28,1,[],['label3','label6']],
        [6,1.6,0.011,28,1,[6],['label4','label7']],
      ]
      var prop_list=["click.price","show.price",
        "space","valid","ad.set","target.label.set"
      ]
      var count=0
      test_entry_list.forEach(function(c){
          prop_list.forEach(function(d,j){
              var key="ad.group:"+mall_id+":"+c[0]+":"+d
              var value=c[j+1]
              if(d.indexOf("set",d.length-3)===-1)
              {
                if(d.indexOf("price",d.length-5)!==-1)
                {
                  var tmpBuffer = new Buffer(8)
                  tmpBuffer.writeDoubleLE(value,0)
                  value= tmpBuffer
                }
                add_used_key(key)
                tair.set(key,value,0,nm,0,function(err,success){
                    should.not.exist(err)
                    success.should.equal(true)
                    count++;
                    if(count==prop_list.length*test_entry_list.length){
                      done()
                    }
                  })
              }
              else
              {
                var zcount=0
                if(c[j+1].length>=1)
                {
                  add_used_key(key)
                }
                else
                {
                  count++
                  if(count==prop_list.length*test_entry_list.length){
                    done()
                  }
                }
                c[j+1].forEach(function(v){
                    tair.sadd(key,nm,v,function(err, data){
                        should.not.exist(err);
                        data.should.equal(true);
                        ++zcount
                        if(zcount==c[j+1].length)
                        {
                          count++ 
                          if(count==prop_list.length*test_entry_list.length){
                            done()
                          }
                        }
                      })
                  })  
              }
            })
        })
    })

  it("c++ test ,ad_request should be ok!",function(done){
      exec(__dirname+'/ad_map_test',function(err,stdout,stderr){
          sys.print('stdout:'+ stdout)
          sys.print('stderr:'+ stderr)
          should.not.exist(err)
          done()
        })
    })

  it("clear up test data should work",function(done){
      var count=0
      for(var key in used_key){
        console.log('remove '+key)
        tair.remove(key,nm,function(err){
          should.not.exist(err)
          count++;
          if(count==Object.keys(used_key).length){
             done();
          }
        })
      }
    })
});
