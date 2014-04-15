
/* JS Page Init */

$("#success_close").click(function(){
	$("#success_alert").hide(400);
});

$("#error_close").click(function(){
	$("#error_alert").hide(400);
});

$(document).ready(function(){
    $('#code').autosize();   
});

/* JS Functions */

function successMsg(title, message) {
	$("#success_alert").hide(0);
	document.getElementById("success_title").innerHTML = "Success: " + title;
	document.getElementById("success_message").innerHTML = message;
	$("#success_alert").show(400);
	window.setTimeout(function() { $("#success_alert").hide(400); }, 3000);
};

function errorMsg(title, message) {
	$("#error_alert").hide(0);
	document.getElementById("error_title").innerHTML = "Error: " + title;
	document.getElementById("error_message").innerHTML = message;
	$("#error_alert").show(400);
};

function remoteExecuteWrapper() {
	// Remove any messages present
	$("#success_alert").hide(200);
	$("#error_alert").hide(200);
	var execute = true;
	var parameters = {
		protocol: "protocol",
		url: "address",
		port: "port",
		username: "username",
		password: "password",
		code: "script"
	};
	for (var param in parameters) {
		if (document.getElementById(param).value == "") {
			errorMsg(
				"Field Not Provided",
				("Please fill in the " + parameters[param] + " field before executing.")
			);
			execute = false;
			break;
		};
	};
	if (execute) {
		remoteExecute(
			((document.getElementById('protocol').value) + (document.getElementById('url').value) + ":" + (document.getElementById('port').value)),
			(document.getElementById('code').value),
			(document.getElementById('username').value),
			(document.getElementById('password').value)
		);
	};
};

function remoteExecute(url, data, username, password) {
	$("#modal_main").hide(0);
	$("#modal_loading").show(0);
	var jqxhr = $.post(url, {
		user: username,
		pass: password,
		script: data
		})
		.done(function() {
			successMsg("Executed SQF Script", "The return value is shown in the output area below.");
		})
		.fail(function(data, statusText, jqXHR) {
			var title = "RCON Request Failed";
			var message = "Something went wrong while communicating with the server!";
			switch (jqXHR) {
				case "":
					title = "Cannot Connect to Server";
					message = "There seems to be network problems between you and the server.";
					break;
				case "Bad Request":
					title = "HTTP 400: Bad Request";
					message = "A field seems to be ommitted or defined improperly.";
					break;
				case "Forbidden":
					title = "HTTP 403: Forbidden";
					message = "Access denied for the given username and/or password.";
					break;
				case "Server Error":
					title = "HTTP 503: Service Unavailable";
					message = "SQF is temporarily unresponsive; most likely the server is currently out-of-mission.";
					break;
			};
			errorMsg(title, message);
		})
		.always(function(data, statusText, jqXHR) {
			if ((typeof data) == "string") {
				document.getElementById('output').value = data;
			};
			$("#modal_loading").hide(0);
			$("#modal_main").show(0);
	});
};
