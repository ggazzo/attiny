var EventEmitter = require('events').EventEmitter;
var options = new EventEmitter();
var inquirer = require('inquirer');

var SerialPort = require('serialport').SerialPort;
var serialPort = require('serialport');



function connect (){
  return new Promise(function(resolve, reject) {
    serialPort.list(function (err, ports) {
      inquirer.prompt([
        {
          type: 'list',
          name: 'port',
          message: 'What port do you want to use?',
          choices : ports.map(function(port) {
            return  port.comName;
          })
        },
        {
          type: 'list',
          name: 'baudrate',
          message: 'What baudrate do you want to use?',
          choices : ['115200', '57600', '38400', '19200', '9600', '4800', '2400', '1800', '1200', '600', '300']
        }
      ]).then(function (answers) {
          options.emit("port_config",answers);
          resolve(answers);
      });
    });
  });
}


var port;
options.on("port_config", function (config) {
  port = new SerialPort(config.port, {
    parser: serialPort.parsers.readline('\n'),
    baudrate: config.baudrate
  });
  port.on('data', function (data) {
    console.log('Data: ' + data);
  });
});
options.on("command", commands);



function commands(){
  inquirer.prompt([
    {
      type: 'list',
      name: 'command',
      message: 'Choose a command to send?',
      choices : ['calibration', 'mpu_initialize', 'read_ypr', 'save_offsets', 'read_offsets','auto_start']
    }
  ]).then(function (answers) {
      port.write(`<${answers.command}>\n`);
      options.emit("command",true);
  });
}

connect().then(commands);
