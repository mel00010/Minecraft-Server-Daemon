hidden = false;
$(document).ready(function() {
	commands = [ "backupServer", "backupAll", "listServers",
		"listOnlinePlayers", "registerOutputListener", "unRegisterOutputListener",
		"restartServer", "restartAll", "sendCommand", "startServer", "serverStatus", "stopServer",
		"stopAll", "stopDaemon", "updateServer", "updateAll", "opPlayer",
		"deopPlayer", "banPlayer", "pardonPlayer", "kickPlayer", "runScript" ];
	commands.forEach(function(item, index, arr) {
		$("#" + item).submit(function(e) {
			e.preventDefault();
			sendCommand();
		});
	});
});
function mode_select() {
	var command_forms = document.forms[1];
	for (i = 0; i < command_forms.length; i++) {
		if (command_forms[i].checked) {
			var selected_command = command_forms[i].value;
			commands.forEach(function(item, index, arr) {
				$("#" + item).hide();
			});
			$("#" + selected_command).show();
			$("#menu").scrollTop($("#menu")[0].scrollHeight);
		}
	}
}
function sendCommand() {
	var command_forms = document.forms[1];
	for (i = 0; i < command_forms.length; i++) {
		if (command_forms[i].checked) {
			var selected_command = command_forms[i].value;
			var data = new Object();
			data.command = selected_command;
			data.server = $("#" + selected_command).find("input[name='SERVER']").val();
			data.player = $("#" + selected_command).find("input[name='PLAYER']").val();
			data.version = $("#" + selected_command).find("input[name='VERSION']").val();
			data.serverCommand = $("#" + selected_command).find("input[name='COMMAND']").val();
			data.reason = $("#" + selected_command).find("input[name='REASON']").val();
			data.script = $("#" + selected_command).find("input[name='SCRIPT']").val();
			//			console.log(data);
			conn.socket.send(JSON.stringify(data));
		}
	}
	return false;
}
function toggleMessageVisibility() {
	if (hidden) {
		$('.outputMessage').show();
		hidden = false;
	} else {
		$('.outputMessage').hide();
		hidden = true;
	}
}