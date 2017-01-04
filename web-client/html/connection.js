var Connection = (function() {

	function Connection(ElementId, url) {
		this.element = document.getElementById('output');
		this.open = false;
		this.registered = false;
		this.numRegistered = 0;
		this.numTotal;
		this.socket = new WebSocket(url);
		this.url = url;
		this.setupConnectionEvents();
	}

	Connection.prototype = {
		addOutputMessage : function(msg) {
			this.element.innerHTML += '<p class="message">' + msg + '</p>\n';
			$("#output").scrollTop($("#output")[0].scrollHeight);
		},


		addSuccessOrFailMessage : function(msg) {
			this.element.innerHTML += '<p class="message outputMessage">' + msg + '</p>\n';
			if (hidden) {
				$('.outputMessage').hide();
			} else {
				$('.outputMessage').show();
			}
			$("#output").scrollTop($("#output")[0].scrollHeight);
		},

		addSystemMessage : function(msg) {
			this.element.innerHTML += '<p class="message"><b>' + msg + '</b></p>\n';
			$("#output").scrollTop($("#output")[0].scrollHeight);
		},

		setupConnectionEvents : function() {
			var self = this;
			console.log()
			self.socket.onopen = function(evt) {
				self.connectionOpen(evt);
			};
			self.socket.onmessage = function(evt) {
				self.connectionMessage(evt);
			};
			self.socket.onclose = function(evt) {
				self.connectionClose(evt);
			};
		},

		connectionOpen : function(evt) {
			this.open = true;
			this.addSystemMessage("Connected");
			if (!this.registered) {
				var data = new Object();
				data.command = "listServers";
				data.server = null;
				data.player = null;
				data.version = null;
				data.serverCommand = null;
				data.reason = null;
				data.script = null;
				this.socket.send(JSON.stringify(data))
			}
		},

		connectionMessage : function(evt) {
			if (!this.open) {
				return;
			} else {
				var message = JSON.parse(evt['data']);
				if (message.mode == "serverOutput") {
					this.addOutputMessage("<b>" + message.server + ":</b>  " + message.serverOutput);
				} else if (message.mode == "output") {
					if (message.messageData != "") {
						if (message.command == "listServers" && !this.registered) {
							var servers = message.messageData.split("\n");
							this.numTotal = servers.length;
							var self = this;
							servers.forEach(function(item, index, arr) {
								var data = new Object();
								data.command = "registerOutputListener";
								data.server = item;
								data.player = null;
								data.version = null;
								data.serverCommand = null;
								data.reason = null;
								data.script = null;
								self.socket.send(JSON.stringify(data))

							});
							if (this.registered) {
								this.addOutputMessage(message.messageData);
							} else {
								this.registered = true;
							}
						} else {
							this.addOutputMessage(message.messageData);
						}
					} else {
						if (message.success == true && this.numRegistered >= this.numTotal) {
							this.addSuccessOrFailMessage("Command " + message.command + " succeeded.");
						} else if (message.success == false && this.numRegistered >= this.numTotal) {

							if (message.failureReason == "") {
								this.addSuccessOrFailMessage("Command " + message.command + " failed.");
							} else {
								this.addSuccessOrFailMessage("Command " + message.command + " failed with reason: \"" + message.failureReason + "\"");
							}
						}
						if (message.command == "registerOutputListener") {
							this.numRegistered++;
						}
					}
				}
			}
		},

		connectionClose : function(evt) {
			this.open = false;
			//this.addSystemMessage("Disconnected");
			var url = this.url;
			setTimeout(function() {
				conn = new Connection('status', url);
			}, 1000);
		},

		sendMsg : function(message) {
			if (this.open) {
				this.socket.send(message);

				this.addOutputMessage(message);
			} else {
				this.addSystemMessage("You are not connected to the server.");
			}
		}
	};

	return Connection;

})();
$(document).ready(function() {
	var loc = window.location,
		new_uri;
	if (loc.protocol === "file:") {
		new_uri = "ws://localhost:8080";
	} else if (loc.protocol === "https:") {
		new_uri = "wss://" + loc.host;
		new_uri += loc.pathname + "websocket";
	} else {
		new_uri = "ws://" + loc.host;
		new_uri += loc.pathname + "websocket";
	}

	conn = new Connection('output', new_uri);
});