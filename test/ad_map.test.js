/*
 * tair - test/client.test.js
 * Copyright(c) 2012 Taobao.com
 * Author: kate.sf <kate.sf@taobao.com>
 */

var cli = require('node-tair-rdb');
var should = require('should');
var sys = require('sys')
var exec = require('child_process').exec
var deepcopy = require('deepcopy');

var tair;
var nm=2;
var used_key={}
var mall_id=2
var mall_level=1

var test_user_list=[
      ["12345678",4.3,3.8,mall_level,['label1','label2','label3']],
      ["23456789",204,100.8,mall_level,['label2','label4','label5']],
      ["34567890",4.3,103.8,mall_level,[]],
      ["45678901",198.3,2.6,mall_level,['label1','label2','label3','label4','label5','label6']],
      ["56789012",100.15,50.3,mall_level,['label4','label5','label7']],
    ]

function save_used_key(keys)
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
      {host: 'WUSHUU-TAIR-CS', port: 5198}
    ], {heartBeatInterval: 3000},
      function (err) {
        if (err) {
          done(err);
          return;
        }
        done();
      });
  });
 
  it("ad_map insert test data users' location & label should be ok!",function(done){
      this.timeout(50000)
      var prop_list=["x","y","z","label.set"]
      var count=0
      test_user_list.forEach(function(c){
          prop_list.forEach(function(d,j){
              var key="location:"+mall_id+":"+c[0]+":"+d
              var value=c[j+1]
              if(d==="x" || d=== "y")
              {
                var tmpBuffer = new Buffer(4)
                tmpBuffer.writeFloatLE(value,0)
                value= tmpBuffer
              }

              if(d==="label.set")
              {
                exec(__dirname+'/get_user_id '+c[0]+' 2>>/dev/null | grep "user\'s id" | awk \'{print $4}\' ',function(err,stdout,stderr){
                    sys.print('stdout:'+ stdout+'\n')
                    sys.print('stderr:'+ stderr+'\n')
                    console.log('get_user_id mac is '+c[0]+', user_id is '+stdout);
                    console.log('error is '+err)
                    should.not.exist(err)
                    var user_id=parseInt(stdout)
                    key="user:"+user_id+":label.set"
                    if(value.length<=0)
                    {
                      count++
                      if(count==prop_list.length*test_user_list.length){
                        done()
                      }
                    }
                    else
                    {
                      save_used_key(key)
                      var zcount=0
                      value.forEach(function(e){
                          tair.hset(key,nm,e,1,function(err, data){
                              should.not.exist(err);
                              data.should.equal(true);
                              ++zcount
                              if(zcount==value.length)
                              {
                                count++ 
                                if(count==prop_list.length*test_user_list.length){
                                  done()
                                }
                              }
                            })
                        })
                    }
                  })
              }
              else
              {
                save_used_key(key)
                tair.set(key,value,0,nm,0,function(err,success){
                    should.not.exist(err)
                    success.should.equal(true)
                    count++;
                    if(count==prop_list.length*test_user_list.length){
                      done()
                    }
                  })
              }
            })
        })
    })

  it("ad_map insert test data ad.space should be ok!",function(done){
      this.timeout(50000)
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
              save_used_key(key)
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
        [1,1.0,0.01,23,1,[1,2,7],['label1','label2', 'label3'],"","",[],95],
        [2,1.1,0.02,23,1,[3],['label1','label3', 'label6'],"","",[],96],
        [3,1.2,0.016,23,1,[4],['label3','label4'],"","",[],97],
        [4,1.4,0.014,28,1,[1,2,5],['label4','label5'],"","",[],98],
        [5,1.5,0.013,28,1,[],['label3','label6'],"1467056789","1467856789",[123456,123457],99],
        [6,1.6,0.011,28,1,[6],['label4','label7'],"","",[],100],
      ]
      var prop_list=["click.price","show.price",
        "space","valid","ad.set","target.label.set","market.start","market.end","market.shop.set","owner"
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
                save_used_key(key)
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
                  save_used_key(key)
                }
                else
                {
                  count++
                  if(count==prop_list.length*test_entry_list.length){
                    done()
                  }
                }
                c[j+1].forEach(function(v){
                    console.log("sadd key="+key+",value="+v)
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

  it("ad_map insert test data ad should be ok!",function(done){
      this.timeout(10000)
      var test_entry_list=[
        [1,1],
        [2,1],
        [3,2],
        [4,3],
        [5,4],
        [6,6],
        [7,1],
      ]
      var prop_list=["group"]
      var count=0

      test_entry_list.forEach(function(c){
          prop_list.forEach(function(d,j){
              var key="ad:"+mall_id+":"+c[0]+":"+d
              var value=c[j+1]
              save_used_key(key)
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

    
  it("ad_map insert test data to ad.location should be ok!",function(done){
    //240*120 => 24*12
    this.timeout(10000)
    var x_len=24
    var y_len=12
    //this is a reverse data
    var test_ad_loc_data=[
      [
        [0,0],[0,1],[0,2],
        [1,1],[1,2],[1,3],
        [2,2],[2,3],[2,4],
        [3,3],[3,4],[3,5],
      ],
      [
        [20,8],[20,9],[20,10],[20,11],
        [21,8],[21,9],[21,10],[21,11],
        [22,8],[22,9],[22,10],[22,11],
        [23,8],[23,9],[23,10],[23,11],
      ],
      [
        [0,8],[0,9],[0,10],[0,11],
        [1,8],[1,9],[1,10],[1,11],
        [2,8],[2,9],[2,10],[2,11],
        [3,8],[3,9],[3,10],[3,11],
      ],
      [
        [20,0],[20,1],[20,2],[20,3],
        [21,0],[21,1],[21,2],[21,3],
        [22,0],[22,1],[22,2],[22,3],
        [23,0],[23,1],[23,2],[23,3],
      ],
      [
        [8,4],[8,5],[8,6],[8,7],
        [9,4],[9,5],[9,6],[9,7],
        [10,4],[10,5],[10,6],[10,7],
        [11,4],[11,5],[11,6],[11,7],
      ],
      [
      ],
    ]
    var full_loc_mark=[]
    for(var i=0;i<x_len;++i)
    {
      full_loc_mark[i]=[]
      for(var j=0;j<y_len;++j)
      {
        full_loc_mark[i][j]=true
      }
    }
    var count=0
    test_ad_loc_data.forEach(function(d,ad_group_index){
      var full_loc_mark_copy=deepcopy(full_loc_mark)
      d.forEach(function(e){
          full_loc_mark_copy[e[0]][e[1]]=false
        })
      var mcount=0
      full_loc_mark_copy.forEach(function(line,i){
        line.forEach(function(mark,j){
            if(mark)
            {
              var key="ad.location:"+mall_id+":"+i+":"+j+":"+mall_level+":ad.group.set"  
              save_used_key(key)
              tair.zadd(key,nm,ad_group_index+1,ad_group_index+1,function(err, data){
                  should.not.exist(err);
                  data.should.equal(true);
                  ++mcount
                  if(mcount==x_len*y_len)
                  {
                    ++count
                  }
                  if(count==test_ad_loc_data.length)
                  {
                    done()
                  }
                })
            }
            else
            {
              ++mcount
              if(mcount==x_len*y_len)
              {
                ++count
              }
              if(count==test_ad_loc_data.length)
              {
                done()
              }
            }
          })
      })
    })



  })
  it("c++ test ,ad_request should be ok!",function(done){
      this.timeout(50000)
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
