// Create a client instance
// client = new Paho.MQTT.Client("127.0.0.1", 9001, "SAMUEL");

client = new Paho.MQTT.Client("192.168.100.50", 9001, "SAMUEL");

// set callback handlers
client.onConnectionLost = onConnectionLost;
client.onMessageArrived = onMessageArrived;

// connect the client
client.connect({ onSuccess: onConnect });


// called when the client connects
function onConnect() {
  // Once a connection has been made, make a subscription and send a message.
  console.log("onConnect");
  client.subscribe("fei/cc1/temperatura/samuel");
  message = new Paho.MQTT.Message();
  message.destinationName = "World";
  client.send(message);
}

// called when the client loses its connection
function onConnectionLost(responseObject) {
  if (responseObject.errorCode !== 0) {
    console.log("onConnectionLost:" + responseObject.errorMessage);
  }
}

// called when a message arrives
function onMessageArrived(message) {
  document.getElementById('recivo').innerText = message.payloadString;
  console.log("onMessageArrived:" + message.payloadString);
}

// function enviar() {
//   var text = document.getElementById('mensaje').value;
//   console.log(text + "hol");
//   console.log("onConnect");
//   client.subscribe("fei/cc1/temperatura/samuel");
//   message = new Paho.MQTT.Message(text);
//   message.destinationName = "World";
//   client.send(message);
//   document.getElementById('text1').innerText = document.getElementById('mensaje').value;
// }
