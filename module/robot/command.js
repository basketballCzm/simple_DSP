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

function _useradd(r,token)
{
    var username=r.parameters.user
    var x = r.parameters.x
    var y = r.parameters.y
  
    _ajax("POST",config.useradd_url,{"user":username,"x":x,"y":y } ,function(){
            console.log("user add returned !!")
        
        })
    
}

exports.useradd= _useradd

