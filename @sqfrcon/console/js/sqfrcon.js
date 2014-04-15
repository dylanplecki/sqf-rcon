
function remoteExecute(url, data, username, password) {
	var jqxhr = $.post(url, {
		user: username,
		pass: password,
		script: data
		})
		.done(function(data) {
			
		})
		.fail(function(data) {
			
		})
		.always(function(data) {
			
	});
};
