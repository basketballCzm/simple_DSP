var config = require('./config')

function _ajax(type, url, data, success, dataType)
{
    'use strict';

    var env = require('jsdom').env
    var html = '<html><body><h1>Hello World!</h1><p class="hello">Heya Big World!</body></html>'

    // first argument can be html string, filename, or url
    env(html, function (errors, window) {
        console.log('errors='+errors);

        var $ = require('jquery')(window)
        $.support.cors = true;
        var XMLHttpRequest = require('xmlhttprequest').XMLHttpRequest;

        $.ajaxSettings.xhr = function() {
            return new XMLHttpRequest();
        }; 

        $.ajax({
            type:type,
            url:url,
            data:data,
            success:success,
            dataType:dataType
        })
        .fail(function(jqXHR, textStatus, errorThrown) {
            console.log("Error: " + errorThrown);
        })

      })
}

function _user_add(r,token)
{
    var id=r.parameters.user
    var x = r.parameters.x
    var y = r.parameters.y
    var z = r.parameters.z
    console.log("id="+id+",x="+x+",y="+y+",z="+z)

    _ajax("GET",config.user_op_url,{"action":"add","user_id":id,"x":x,"y":y,"z":z } ,function(result){
            console.log("user add returned ,result:"+result)
        })
    
}

function _user_update(r,token)
{
    var id=r.parameters.user
    var x = r.parameters.x
    var y = r.parameters.y
    var z = r.parameters.z
  
    _ajax("GET",config.user_op_url,{"action":"update","user_id":id,"x":x,"y":y,"z":z } ,function(result){
            console.log("user update returned ,result:"+result)
        })
    
}

function _user_remove(r,token)
{
    var id=r.parameters.user
  
    _ajax("GET",config.user_op_url,{"action":"remove","user_id":id } ,function(result){
            console.log("user remove returned ,result:"+result)
        })
    
}

function _help(r, token) {
    console.log(r.help());
}

exports.user_add= _user_add
exports.user_update= _user_update
exports.user_remove= _user_remove
exports.help=_help
