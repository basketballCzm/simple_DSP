var command =require('./command')

var meta = {
    program : 'robot',
    name : 'robot of customer',
    description : 'robot to simulate customer in the mall',
    version : '0.0.1',
    subcommands : [ 'useradd', 'userdel', 'relocate', 'ad_request', 'ad_click' ], 
    options : {
        flags : [
            /* short_name, name, description */
            [ 'h', 'help', 'print program usage' ],
        ],
        parameters : [
            /* short_name, name, description, default_value */
            [ 'u', 'user', 'username', null ],
            [ 'x', 'x-axis', 'x-axis value of the coordinate plane', 0 ],
            [ 'y', 'y-axis', 'y-axis value of the coordinate plane', 0 ],
        ]
    },
    usages : [
        /* subcommand, options, positional-arguments, description, handler */
        ['useradd', null, ['user','[x-axis]','[y-axis]'], 'add new user', command.useradd]
    ]
};

var lineparser = require('lineparser');



exports.parser = lineparser.init(meta);
exports.useradd_url = "http://www.baidu.com"
