var command =require('./command')

var meta = {
    program : 'robot',
    name : 'robot of customer',
    description : 'robot to simulate customer in the mall',
    version : '0.0.1',
    subcommands : [ 'user_add', 'user_remove', 'user_update', 'ad_request', 'ad_click' ], 
    options : {
        flags : [
            /* short_name, name, description */
            [ 'h', 'help', 'print program usage' ],
        ],
        parameters : [
            /* short_name, name, description, default_value */
            [ 'u', 'user', 'user id', null ],
            [ 'x', 'x-axis', 'x-axis value of the coordinate plane', 0 ],
            [ 'y', 'y-axis', 'y-axis value of the coordinate plane', 0 ],
            [ 'z', 'z-axis', 'z-axis value of the coordinate plane', 0 ],
        ]
    },
    usages : [
        /* subcommand, options, positional-arguments, description, handler */
        ['user_add', null, ['user','[x-axis]','[y-axis]','[z-axis]'], 'add new user', command.user_add],
        ['user_remove', null, ['user'], 'add new user', command.user_remove],
        ['user_update', null, ['user','[x-axis]','[y-axis]','[z-axis]'], "update user's location", command.user_update],
        [ null, ['h'], null, 'help', command.help ],
        [ null, null, null, 'help', command.help ]
    ]
};

var lineparser = require('lineparser');



exports.parser = lineparser.init(meta);
exports.user_op_url = "http://115.28.9.186:8000/user_op.cgi"
